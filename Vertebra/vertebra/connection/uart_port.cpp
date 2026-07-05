#include "uart_port.hpp"

#include "callback.hpp"
#include "vertebra/components.hpp"
#include "vertebra/defs.hpp"
#include "vertebra/error.hpp"

#ifdef HAL_UART_MODULE_ENABLED

namespace vtb::uart
{

std::vector<PortBase *> PortBase::ports_;

PortBase::PortBase(Handle & huart, uint8_t * buffer, uint32_t size)
: huart_(huart), buffer_(buffer), size_(size), half_(size / 2U)
{
}

PortBase::operator bool() const { return status_.enable; }

void PortBase::awake()
{
  check_status();
  if (status_.dma.tx) {
    tx_ = &dma_tx;
  } else if (status_.interrupt.tx) {
    tx_ = &it_tx;
  } else {
    tx_ = &bl_tx;
  }

  if (!status_.interrupt.rx) {
    Error::handle_error(ErrorType::INIT_FAILED);
  } else if (!status_.dma.rx) {
    launch_rx_ = HAL_UART_Receive_IT;
  } else if (!status_.dma.circular) {
    launch_rx_ = HAL_UART_Receive_DMA;
  }

  reset_rx();
}

const USART_TypeDef * PortBase::get_instance() const { return huart_.Instance; }

bool PortBase::transmit(const uint8_t * data, size_t len) const { return tx_(&huart_, data, len); }

bool PortBase::transmit_blocking(const uint8_t * data, size_t len, uint32_t timeout) const
{
  return HAL_UART_Transmit(&huart_, data, len, timeout) == HAL_OK;
}

void PortBase::add_tx_callback(Callback callback) { tx_callbacks_.emplace_back(callback); }

void PortBase::add_rx_callback(Callback callback) { rx_callbacks_.emplace_back(callback); }

void PortBase::check_status()
{
  uint32_t cr1 = READ_REG(huart_.Instance->CR1);
  uint32_t cr3 = READ_REG(huart_.Instance->CR3);
  uint32_t dma_cr = get_dma_cr();

  // clang-format off
  status_.enable        = HAL_IS_BIT_SET(cr1, USART_CR1_UE);
  status_.interrupt.tx  = HAL_IS_BIT_SET(cr1, USART_CR1_TXEIE);
  status_.interrupt.tc  = HAL_IS_BIT_SET(cr1, USART_CR1_TCIE);
  status_.interrupt.rx  = HAL_IS_BIT_SET(cr1, USART_CR1_RXNEIE);
  status_.idle          = HAL_IS_BIT_SET(cr1, USART_CR1_IDLEIE);
  status_.dma.tx        = HAL_IS_BIT_SET(cr3, USART_CR3_DMAT);
  status_.dma.rx        = HAL_IS_BIT_SET(cr3, USART_CR3_DMAR);
  status_.dma.circular  = dma_is_circular(dma_cr);
  // clang-format on
}

uint32_t PortBase::get_dma_cr() const
{
#if defined(VTB_DMA_CHANNEL)
  return huart_.hdmarx ? READ_REG(huart_.hdmarx->Instance->CCR) : 0;
#elif defined(VTB_DMA_STREAM)
  return huart_.hdmarx ? READ_REG(huart_.hdmarx->Instance->CR) : 0;
#endif
}

bool PortBase::dma_is_circular(uint32_t cr) const
{
#if defined(VTB_DMA_CHANNEL)
  return HAL_IS_BIT_SET(cr, DMA_CCR_CIRC);
#elif defined(VTB_DMA_STREAM)
  return HAL_IS_BIT_SET(cr, DMA_SxCR_CIRC);
#endif
}

void PortBase::reset_rx()
{
  if (buf_front_half_) {
    launch_rx_(&huart_, buffer_ + half_, half_);
    buf_front_half_ = false;
  } else {
    launch_rx_(&huart_, buffer_, half_);
    buf_front_half_ = true;
  }
}

bool PortBase::bl_tx(Handle * handle, const uint8_t * data, size_t len)
{
  auto state = HAL_UART_Transmit(handle, data, len, HAL_MAX_DELAY);
  HAL_UART_TxCpltCallback(handle);
  return state == HAL_OK;
}

bool PortBase::it_tx(Handle * handle, const uint8_t * data, size_t len)
{
  return HAL_UART_Transmit_IT(handle, data, len) == HAL_OK;
}

bool PortBase::dma_tx(Handle * handle, const uint8_t * data, size_t len)
{
  return HAL_UART_Transmit_DMA(handle, data, len) == HAL_OK;
}

void PortBase::notify_tx(Handle * huart)
{
  for (PortBase * port : ports_) {
    if (huart->Instance == port->get_instance()) {
      port->exec_tx_callbacks();
    }
  }
}

void PortBase::notify_rx(Handle * huart)
{
  for (PortBase * port : ports_) {
    if (huart->Instance == port->get_instance()) {
      port->exec_rx_callbacks();
    }
  }
}

void PortBase::notify_rxh(Handle * huart)
{
  for (PortBase * port : ports_) {
    if (huart->Instance == port->get_instance()) {
      port->exec_rxh_callbacks();
    }
  }
}

void PortBase::notify_idle(Handle * huart, uint16_t size)
{
  for (PortBase * port : ports_) {
    if (huart->Instance == port->get_instance()) {
      port->exec_idle_callbacks(size);
    }
  }
}

void PortBase::exec_tx_callbacks()
{
  for (const auto & callback : tx_callbacks_) {
    callback.call();
  }
}

void PortBase::exec_rx_callbacks()
{
  RcvData rcv = {buffer_ + half_, half_, status_.idle};
  if (buf_front_half_) rcv.data = buffer_;
  if (!status_.dma.circular) reset_rx();
  for (const auto & callback : rx_callbacks_) {
    callback.call(rcv);
  }
}

void PortBase::exec_rxh_callbacks()
{
  if (!status_.dma.circular) return;
  buf_front_half_ = false;
  RcvData rcv = {buffer_, half_, status_.idle};
  for (const auto & callback : rx_callbacks_) {
    callback.call(rcv);
  }
}

void PortBase::exec_idle_callbacks(uint16_t size)
{
  RcvData rcv = {buffer_, size, false};
  if (!buf_front_half_) rcv.data = buffer_ + half_;
  if (size > half_) rcv.size = size - half_;
  if (status_.dma.circular) {
    switch_buffer();
  } else {
    reset_rx();
  }
  for (const auto & callback : rx_callbacks_) {
    callback.call(rcv);
  }
}

/*
  It only happens when the IDLE interrupt is enabled and the DMA is in the CIRCULAR mode.
  It is expected that the duration of IDLE is longer than the execution of this function,
  otherwise you might lose incoming data.
  However, the special case above can hardly happen in the normal situation.
*/
void PortBase::switch_buffer()
{
  volatile uint32_t * mar = nullptr;
  volatile uint32_t * ndtr = nullptr;
#if defined(VTB_DMA_CHANNEL)
  mar = &huart_.hdmarx->Instance->CMAR;
  ndtr = &huart_.hdmarx->Instance->CNDTR;
#elif defined(VTB_DMA_STREAM)
  mar = &huart_.hdmarx->Instance->M0AR;
  ndtr = &huart_.hdmarx->Instance->NDTR;
#endif
  if (!mar) return;
  uint32_t half_addr = (uint32_t)(buffer_ + half_);
  __HAL_DMA_DISABLE(huart_.hdmarx);
  if (*mar < half_addr) {
    *mar = half_addr;
    *ndtr = half_;
    buf_front_half_ = false;
  } else {
    *mar = (uint32_t)buffer_;
    *ndtr = size_;
    buf_front_half_ = true;
  }
  __HAL_DMA_ENABLE(huart_.hdmarx);
}

}  // namespace vtb::uart

extern "C" {

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart) { vtb::uart::PortBase::notify_tx(huart); }

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart) { vtb::uart::PortBase::notify_rx(huart); }

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef * huart) { vtb::uart::PortBase::notify_rxh(huart); }

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t size)
{
  if (huart->RxEventType == HAL_UART_RXEVENT_HT) {
    vtb::uart::PortBase::notify_rxh(huart);
    return;
  }
  if (huart->RxEventType == HAL_UART_RXEVENT_TC) {
    vtb::uart::PortBase::notify_rx(huart);
    return;
  }
  vtb::uart::PortBase::notify_idle(huart, size);
}
}

#endif