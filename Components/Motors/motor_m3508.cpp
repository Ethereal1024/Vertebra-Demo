#include "motor_m3508.hpp"

#include <sys/types.h>

M3508Group::M3508Group(const CAN_HandleTypeDef& hcan)
    : can_manager_(std::make_unique<CANMsgManager>(hcan)) {
  can_manager_->set_frame_id(0x200)
      .set_frame_type(CANFrameType::Standard)
      .set_remote_type(CANRemoteType::Data)
      .set_data_len(8);
}

void M3508Group::force(int id, float strength) {
  if (id < 1 || id > 4) return;
  motors_[id - 1] = strength;
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

MotorM3508::MotorM3508(M3508Group& group, int motor_id)
    : group_(group), motor_id_(motor_id) {}

void MotorM3508::force(float strength) { group_.force(motor_id_, strength); }