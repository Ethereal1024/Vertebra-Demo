#ifndef MOTOR_M3508
#define MOTOR_M3508

#include <memory>

#include "bridge/can_msg_manager.hpp"
#include "main.h"
#include "motor.hpp"

class M3508Group {
 public:
  M3508Group() = delete;
  M3508Group(const CAN_HandleTypeDef& hcan);
  void force(int id, float strength);

 private:
  void send();
  std::unique_ptr<CANMsgManager> can_manager_;
  int16_t motors_[4];
};

class MotorM3508 : public MotorBase {
 public:
  MotorM3508() = delete;
  MotorM3508(M3508Group& group, int motor_id);
  void force(float strenghth) override;

 private:
  M3508Group& group_;
  int motor_id_;
};

#endif