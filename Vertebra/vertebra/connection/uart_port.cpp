#include "uart_port.hpp"

#include "callback.hpp"
#include "vertebra/components.hpp"
#include "vertebra/defs.hpp"
#include "vertebra/error.hpp"

#ifdef HAL_UART_MODULE_ENABLED

namespace vtb::uart
{

std::vector<Port *> Port::ports_;

Port::Port(Handle & huart, uint8_t * buffer, uint32_t size)
: huart_(huart), buffer_(buffer), size_(size), half_(size / 2U)
{
}

Port::operator bool() const { return status_.enable; }

void Port::awake()
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
    reset_rx_ = reset_rx_it;
  } else if (!status_.dma.circular) {
    reset_rx_ = reset_rx_dma;
  }

  reset_rx_(this);
}

const USART_TypeDef * Port::get_instance() const { return huart_.Instance; }

bool Port::transmit(const uint8_t * data, size_t len) { return tx_(&huart_, data, len); }

bool Port::transmit_blocking(const uint8_t * data, size_t len, uint32_t timeout = HAL_MAX_DELAY)
{
  return HAL_UART_Transmit(&huart_, data, len, timeout) == HAL_OK;
}

void Port::add_tx_callback(Callback callback) { tx_callbacks_.emplace_back(callback); }

void Port::add_rx_callback(Callback callback) { tx_callbacks_.emplace_back(callback); }

void Port::check_status()
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

uint32_t Port::get_dma_cr() const
{
#if defined(VTB_DMA_CHANNEL)
  return huart_.hdmarx ? READ_REG(huart_.hdmarx->Instance->CCR) : 0;
#elif defined(VTB_DMA_STREAM)
  return huart_.hdmarx ? READ_REG(huart_.hdmarx->Instance->CR) : 0;
#endif
}

bool Port::dma_is_circular(uint32_t cr) const
{
#if defined(VTB_DMA_CHANNEL)
  return HAL_IS_BIT_SET(cr, DMA_CCR_CIRC);
#elif defined(VTB_DMA_STREAM)
  return HAL_IS_BIT_SET(cr, DMA_SxCR_CIRC);
#endif
}

void Port::reset_rx_it(Port * port)
{
  if (port->buf_half_used_) {
    HAL_UART_Receive_IT(&port->huart_, port->buffer_ + port->half_, port->half_);
    port->buf_half_used_ = false;
  } else {
    HAL_UART_Receive_IT(&port->huart_, port->buffer_, port->half_);
    port->buf_half_used_ = true;
  }
}

void Port::reset_rx_dma(Port * port)
{
  HAL_UART_Receive_DMA(&port->huart_, port->buffer_, port->size_);
}

bool Port::bl_tx(Handle * handle, const uint8_t * data, size_t len)
{
  auto state = HAL_UART_Transmit(handle, data, len, HAL_MAX_DELAY);
  HAL_UART_TxCpltCallback(handle);
  return state == HAL_OK;
}

bool Port::it_tx(Handle * handle, const uint8_t * data, size_t len)
{
  return HAL_UART_Transmit_IT(handle, data, len) == HAL_OK;
}

bool Port::dma_tx(Handle * handle, const uint8_t * data, size_t len)
{
  return HAL_UART_Transmit_DMA(handle, data, len) == HAL_OK;
}

void Port::notify_tx(Handle * huart)
{
  for (Port * port : ports_) {
    if (huart->Instance == port->get_instance()) {
      port->exec_tx_callbacks();
    }
  }
}

void Port::notify_rx(Handle * huart)
{
  for (Port * port : ports_) {
    if (huart->Instance == port->get_instance()) {
      port->exec_rx_callbacks();
    }
  }
}

void Port::notify_rxh(Handle * huart)
{
  for (Port * port : ports_) {
    if (huart->Instance == port->get_instance()) {
      port->exec_rxh_callbacks();
    }
  }
}

void Port::notify_idle(Handle * huart, uint16_t size)
{
  for (Port * port : ports_) {
    if (huart->Instance == port->get_instance()) {
      port->exec_idle_callbacks(size);
    }
  }
}

void Port::exec_tx_callbacks()
{
  for (const auto & callback : rx_callbacks_) {
    callback.call();
  }
}

void Port::exec_rx_callbacks()
{
  RcvData rcv = {buffer_ + half_, half_, status_.idle};
  if (!status_.dma.rx && buf_half_used_) rcv.data = buffer_;
  if (!status_.dma.circular) reset_rx_(this);
  for (const auto & callback : rx_callbacks_) {
    callback.call(rcv);
  }
}

void Port::exec_rxh_callbacks()
{
  if (!status_.dma.rx) return;
  buf_half_used_ = true;
  RcvData rcv = {buffer_, half_, status_.idle};
  for (const auto & callback : rx_callbacks_) {
    callback.call(rcv);
  }
}

void Port::exec_idle_callbacks(uint16_t size)
{
  if (!status_.dma.rx) {
    reset_rx_it(this);
  } else {
    switch_buffer();
  }
}

/*
  It only happens when the IDLE interrupt is enabled and the DMA is in the CIRCULAR mode.
  It is expected that the duration of IDLE is longer than the execution of this function,
  otherwise you might lose incoming data.
  However, the special case above can hardly happen in the normal situation.
*/
void Port::switch_buffer() const
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
  } else {
    *mar = (uint32_t)buffer_;
    *ndtr = size_;
  }
  __HAL_DMA_ENABLE(huart_.hdmarx);
}

}  // namespace vtb::uart

extern "C" {

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart) { vtb::uart::Port::notify_tx(huart); }

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart) { vtb::uart::Port::notify_rx(huart); }

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef * huart) { vtb::uart::Port::notify_rxh(huart); }

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef * huart, uint16_t size)
{
  if (huart->RxEventType == HAL_UART_RXEVENT_HT) {
    vtb::uart::Port::notify_rxh(huart);
    return;
  }
  if (huart->RxEventType == HAL_UART_RXEVENT_TC) {
    vtb::uart::Port::notify_rx(huart);
    return;
  }
}
}

#endif