#ifndef VTB_UART_SENDER
#define VTB_UART_SENDER

#include "main.h"

#ifdef HAL_UART_MODULE_ENABLED

#include "uart_port.hpp"

namespace vtb::uart
{

class Sender
{
public:
  explicit Sender(PortBase & port);

  void send(const uint8_t * data, size_t len);
  void send_async(const uint8_t * data, size_t len);

  void register_tx_callback(Callback callback);

private:
  PortBase & port_;
};

}  // namespace vtb::uart

#endif
#endif