#ifndef MOTOR_M3508
#define MOTOR_M3508

#include <memory>

#include "connection/can_port.hpp"
#include "connection/can_receiver.hpp"
#include "connection/can_sender.hpp"
#include "main.h"
#include "motor.hpp"

namespace vtb
{
class M3508Group
{
public:
  explicit M3508Group(can::Port & port);

  template <uint8_t ID>
  can::Receiver create_receiver(const std::function<void(const can::RcvData &)> & callback);

  template <uint8_t ID>
  void force(float strength);

private:
  void send(bool greater);

  can::Port & port_;

  can::Sender sender0_;
  can::Sender sender1_;

  int16_t strenghths[8];
};

template <uint8_t ID>
class MotorM3508 : public Motor
{
public:
  explicit MotorM3508(M3508Group & group);

  void force(float strenghth) override;
  float get_angle() override;
  float get_speed() override;

  float get_current();
  float get_temperature();

  void callback(const can::RcvData & rcv);

private:
  M3508Group & group_;
  can::Receiver receiver_;

  int16_t angle_;
  int16_t speed_;
  int16_t current_;
  int8_t temperature_;
};

}  // namespace vtb

#endif