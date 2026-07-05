#include "uart_sender.hpp"
#include "vertebra/error.hpp"

#ifdef HAL_UART_MODULE_ENABLED

namespace vtb::uart
{

Sender::Sender(const PortBase & port) : port_(port) {}

void Sender::send(const uint8_t * data, size_t len) {
  if (!port_.transmit(data, len)) {
    Error::handle_error();
  }
}

}

#endif