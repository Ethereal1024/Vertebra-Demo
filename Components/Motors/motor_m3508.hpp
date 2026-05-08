#ifndef MOTOR_M3508
#define MOTOR_M3508

#include <memory>

#include "bridge/can_msg_manager.hpp"
#include "main.h"
#include "motor.hpp"

class M3508Group {
 public:
  explicit M3508Group(const CANHandle& hcan);

  template <uint8_t ID>
  void force(float strength);

 private:
  void send();
  std::unique_ptr<CANMsgManager> can_manager_;
  int16_t motors_[4];
};

template <uint8_t ID>
class MotorM3508 : public MotorBase {
 public:
  explicit MotorM3508(M3508Group& group);
  void force(float strenghth) override;

 private:
  M3508Group& group_;
};

#endif