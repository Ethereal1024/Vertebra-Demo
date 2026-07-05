#include "uart_receiver.hpp"

#ifdef HAL_UART_MODULE_ENABLED

namespace vtb::uart
{

Receiver::Receiver(PortBase & port, Callback callback) : port_(port), callback_(callback) {
  port_.add_rx_callback(callback_);
}

}

#endif