#ifndef MOTOR_M3508
#define MOTOR_M3508

#include "main.h"
#include "motor.hpp"

class MotorM3508 : public MotorBase {
 public:
  MotorM3508() = delete;
  MotorM3508(CAN_HandleTypeDef* hcan);
  void force(float strenghth) override;

 private:
  CAN_HandleTypeDef* hcan_;
};

#endif