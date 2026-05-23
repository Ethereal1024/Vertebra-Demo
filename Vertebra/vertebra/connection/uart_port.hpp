#ifndef VTB_UART_PORT
#define VTB_UART_PORT

#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

#include "vertebra/design/loon.hpp"

namespace vtb::uart
{

using Handle = UART_HandleTypeDef;

class Port : public Loon
{
public:
  explicit Port(Handle & huart);

  void awake() override;

  const USART_TypeDef * get_instance() const;

  bool transmit(const uint8_t * data, size_t len);

private:
  Handle & huart_;
};

}  // namespace vtb::uart

#endif

#endif