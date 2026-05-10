#ifndef MOTOR_M3508
#define MOTOR_M3508

#include <memory>

#include "bridge/can_port.hpp"
#include "bridge/can_receiver.hpp"
#include "bridge/can_sender.hpp"
#include "main.h"
#include "motor.hpp"

class M3508Group
{
public:
  explicit M3508Group(CAN::Port & port);

  template <uint8_t ID>
  CAN::Receiver create_receiver(const std::function<void(const CAN::RcvData &)> & callback);

  template <uint8_t ID>
  void force(float strength);

private:
  void send(bool greater);

  CAN::Port & port_;

  CAN::Sender sender0_;
  CAN::Sender sender1_;

  int16_t strenghths[8];
};

template <uint8_t ID>
class MotorM3508 : public MotorBase
{
public:
  explicit MotorM3508(M3508Group & group);

  void force(float strenghth) override;
  float get_angle() override;
  float get_speed() override;

  float get_current();
  float get_temperature();

  void callback(const CAN::RcvData& rcv);

private:
  M3508Group & group_;
  CAN::Receiver receiver_;

  int16_t angle_; 
  int16_t speed_;
  int16_t current_;
  int8_t temperature_;
};

#endif