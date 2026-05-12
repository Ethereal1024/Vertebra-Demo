#include "motor_m3508.hpp"

#ifdef HAL_CAN_MODULE_ENABLED

#include <math.h>
#include <sys/types.h>

#include <cmath>

#include "connection/can_port.hpp"
#include "defs.hpp"

using std::placeholders::_1;

namespace vtb
{

M3508Group::M3508Group(can::Port & port) : port_(port), sender0_(port), sender1_(port)
{
  sender0_.set_frame_id(0x200)
    .set_frame_type(can::FrameType::Standard)
    .set_remote_type(can::RemoteType::Data)
    .set_data_len(8);
  sender1_.set_frame_id(0x1FF)
    .set_frame_type(can::FrameType::Standard)
    .set_remote_type(can::RemoteType::Data)
    .set_data_len(8);
}

template <uint8_t ID>
can::Receiver M3508Group::create_receiver(
  const std::function<void(const can::RcvData &)> & callback)
{
  uint32_t frame_id = 0x200 | ID;
  return can::Receiver(port_, frame_id, callback);
}

template <uint8_t ID>
void M3508Group::force(float strength)
{
  constexpr float raw = 16384.0f;
  constexpr float amp = 20.0f;
  constexpr float scale = raw / amp;

  float input = std::clamp(strength * scale, -raw, raw);
  motors_[ID - 1] = static_cast<int16_t>(std::lround(input));
  if (ID <= 4)
    send(false);
  else
    send(true);
}

void M3508Group::send(bool greater)
{
  uint8_t data[8];
  int offset = 0;
  can::Sender * sender = &sender0_;
  if (greater) {
    offset = 4;
    sender = &sender1_;
  }
  for (size_t i = 0; i < 4; i++) {
    size_t j = i * 2;
    size_t k = i + offset;
    data[j] = strenghths[k] >> 8;
    data[j + 1] = strenghths[k];
  }
  sender->send(data);
}

template <uint8_t ID>
MotorM3508<ID>::MotorM3508(M3508Group & group)
: group_(group), receiver_(group.create_receiver<ID>(std::bind(&callback, this, _1)))
{
  static_assert(ID >= 1 && ID <= 8, "M3508 Motor ID must be the int between 1 and 8");
}

template <uint8_t ID>
void MotorM3508<ID>::force(float strength)
{
  group_.force<ID>(strength);
}

template <uint8_t ID>
float MotorM3508<ID>::get_angle()
{
  constexpr float raw = 8191.0f;
  constexpr float scale = 2 * PI / raw;
  constexpr float offset = -PI;
  return angle_ * scale + offset;
}

template <uint8_t ID>
float MotorM3508<ID>::get_speed()
{
  constexpr float scale = 2 * PI / 60.0f;
  return speed_ * scale;
}

template <uint8_t ID>
float MotorM3508<ID>::get_current()
{
  return current_;
}

template <uint8_t ID>
float MotorM3508<ID>::get_temperature()
{
  return temperature_;
}

template <uint8_t ID>
void MotorM3508<ID>::callback(const can::RcvData & rcv)
{
  angle_ = ((rcv.data[0] << 8) | rcv.data[1]);
  speed_ = ((rcv.data[2] << 8) | rcv.data[3]);
  current_ = ((rcv.data[4] << 8) | rcv.data[5]);
  temperature_ = rcv.data[6];
}

}  // namespace vtb

#endif