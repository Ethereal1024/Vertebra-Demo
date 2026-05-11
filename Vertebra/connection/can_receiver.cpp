#include "can_receiver.hpp"

namespace vtb::can
{
Receiver::Receiver(
  Port & port, uint8_t frame_id, std::function<void(const RcvData &)> callback, bool extended)
: frame_id_(frame_id), port_(port), callback_(callback)
{
  if (extended)
    port_.add_ext_callback(frame_id_, callback_);
  else
    port_.add_std_callback(frame_id_, callback_);
}

uint8_t Receiver::get_frame_id() { return frame_id_; }
}  