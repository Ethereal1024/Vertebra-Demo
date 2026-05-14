#include "can_receiver.hpp"

#ifdef HAL_CAN_MODULE_ENABLED

namespace vtb::can
{
Receiver::Receiver(
  Port & port, uint32_t frame_id, std::function<void(const RcvData &)> callback, bool extended)
: frame_id_(frame_id), port_(port), callback_(callback)
{
  if (extended)
    port_.add_ext_callback(frame_id_, callback_);
  else
    port_.add_std_callback(frame_id_, callback_);
}

uint32_t Receiver::get_frame_id() { return frame_id_; }
}

#endif