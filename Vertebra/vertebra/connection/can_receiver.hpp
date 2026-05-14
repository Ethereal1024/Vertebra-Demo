#ifndef VTB_CAN_RECEIVER
#define VTB_CAN_RECEIVER

#include "main.h"

#ifdef HAL_CAN_MODULE_ENABLED

#include <functional>

#include "can_port.hpp"

namespace vtb::can
{

class Receiver
{
public:
  explicit Receiver(
    Port & port, uint32_t frame_id, std::function<void(const RcvData &)> callback,
    bool extended = false);

  uint32_t get_frame_id();

private:
  uint32_t frame_id_;
  Port & port_;
  const std::function<void(const RcvData &)> callback_;
};

}  // namespace vtb::can

#endif

#endif