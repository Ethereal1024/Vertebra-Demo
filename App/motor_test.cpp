#include "can.h"
#include "usart.h"
#include "vertebra/algorithm/pid.hpp"
#include "vertebra/connection/can.hpp"
#include "vertebra/connection/uart.hpp"
#include "vertebra/device/motor/motor_m3508.hpp"
#include "vertebra/task/cycle_task.hpp"
#include "vertebra/tool/switch_buffer.hpp"

vtb::can::Port can_port(hcan1);
vtb::PidCtrl angle_pid_ctrl(0.5, 0.01, 0.0);
vtb::PidCtrl speed_pid_ctrl(0.5, 0.01, 0.0);
vtb::M3508Group mgroup(can_port);
vtb::MotorM3508<2> motor(mgroup);

float angle;
float speed;
float current;

class MotorPlot : public vtb::CycleTask<1024>
{
public:
  explicit MotorPlot(uint32_t delay, const char * name) : CycleTask<1024>(delay, name) {}

  void start() override
  {
    motor.force(0);
    speed_pid_ctrl.set_integral(VTB_PID_INTEGRAL_DETACH(1000.0f))
      .set_pre(VTB_PID_PRE_INTEGRAL_LIMIT(50));
  }

  void update() override
  {
    angle = motor.get_angle();
    speed = motor.get_speed();
    // float inst_speed = angle_pid_ctrl.instruct(angle, 1.57, get_delay());
    float inst = speed_pid_ctrl.instruct(speed, 5.0, get_delay());
    motor.force(inst);
  }
};

MotorPlot motor_plot(1, "plot");