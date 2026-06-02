#include "uart_port.hpp"

#include "callback.hpp"
#include "stm32f407xx.h"
#include "stm32f4xx_hal_uart.h"

#ifdef HAL_UART_MODULE_ENABLED

namespace vtb::uart
{

std::vector<Port *> Port::ports_;

Port::Port(Handle & huart, uint8_t * buffer, uint32_t size)
: huart_(huart), buffer_(buffer), size_(size)
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

  if (status_.dma.rx) {}
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

  // clang-format off
    status_.enable        = HAL_IS_BIT_SET(cr1, USART_CR1_UE);
    status_.interrupt.tx  = HAL_IS_BIT_SET(cr1, USART_CR1_TXEIE);
    status_.interrupt.tc  = HAL_IS_BIT_SET(cr1, USART_CR1_TCIE);
    status_.interrupt.rx  = HAL_IS_BIT_SET(cr1, USART_CR1_RXNEIE);
    status_.idle          = HAL_IS_BIT_SET(cr1, USART_CR1_IDLEIE);
    status_.dma.tx        = HAL_IS_BIT_SET(cr3, USART_CR3_DMAT);
    status_.dma.rx        = HAL_IS_BIT_SET(cr3, USART_CR3_DMAR);
  // clang-format on
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

void Port::notify_tx(UART_HandleTypeDef * huart)
{
  for (const Port * port : ports_) {
    if (huart->Instance == port->get_instance()) {
      port->exec_tx_callbacks();
    }
  }
}

void Port::notify_rx(UART_HandleTypeDef * huart)
{
  for (const Port * port : ports_) {
    if (huart->Instance == port->get_instance()) {
      port->exec_rx_callbacks();
    }
  }
}

void Port::exec_tx_callbacks() const
{
  for (const auto & callback : rx_callbacks_) {
    callback.call();
  }
}

void Port::exec_rx_callbacks() const
{
  RcvData rcv = {buffer_, size_};
  for (const auto & callback : rx_callbacks_) {
    callback.call(rcv);
  }
}

}  // namespace vtb::uart

extern "C" {

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart) { vtb::uart::Port::notify_tx(huart); }

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart) { vtb::uart::Port::notify_rx(huart); }
}

#endif