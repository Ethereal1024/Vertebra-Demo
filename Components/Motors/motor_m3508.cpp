#include "motor_m3508.hpp"

#include <sys/types.h>

M3508Group::M3508Group(const CANHandle& hcan)
    : can_manager_(std::make_unique<CANMsgManager>(hcan)) {
  can_manager_->set_frame_id(0x200)
      .set_frame_type(CANFrameType::Standard)
      .set_remote_type(CANRemoteType::Data)
      .set_data_len<8>();
}

template <uint8_t ID>
void M3508Group::force(float strength) {
  motors_[ID - 1] = strength;
  send();
}

void M3508Group::send() {
  uint8_t data[8];
  for (size_t i = 0; i < 4; i++) {
    size_t j = i * 2;
    data[j] = motors_[i] >> 8;
    data[j + 1] = motors_[i];
  }
  can_manager_->send_data(data);
}

template <uint8_t ID>
MotorM3508<ID>::MotorM3508(M3508Group& group) : group_(group) {
  static_assert(
      ID >= 1 && ID <= 4,
      "M3508 Motor ID must be the int greater than 1 and less than 4");
}

template <uint8_t ID>
void MotorM3508<ID>::force(float strength) {
  group_.force(strength);
}