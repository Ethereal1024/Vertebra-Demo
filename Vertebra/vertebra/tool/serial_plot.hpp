#ifndef VTB_SERIAL_PLOT
#define VTB_SERIAL_PLOT

#include "main.h"
#include "switch_buffer.hpp"

#ifdef HAL_UART_MODULE_ENABLED

#include <cstdint>
#include <vector>

#include "vertebra/connection/uart.hpp"

namespace vtb
{

template <typename T>
class SerialPlot
{
public:
  explicit SerialPlot(uart::PortBase & port, Callback callback)
  : sender_(port), receiver_(port, callback)
  {
  }
  T & interface() { return buffer_.interface(); }

private:
  uart::Sender sender_;
  uart::Receiver receiver_;
  SwitchBuffer<T> buffer_;
};

}  // namespace vtb

#endif
#endif