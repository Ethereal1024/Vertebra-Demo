#ifndef VTB_UART_RECEIVER
#define VTB_UART_RECEIVER

#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

#include "callback.hpp"
#include "uart_port.hpp"

namespace vtb::uart
{

class Receiver
{
public:
  explicit Receiver(PortBase & port, Callback callback);

private:
  PortBase & port_;
  const Callback callback_;
};

}  // namespace vtb::uart

#endif
#endif