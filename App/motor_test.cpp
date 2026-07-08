#include "can.h"
#include "usart.h"
#include "vertebra/connection/can.hpp"
#include "vertebra/connection/uart.hpp"
#include "vertebra/device/motor/motor_m3508.hpp"
#include "vertebra/task/cycle_task.hpp"
#include "vertebra/tool/switch_buffer.hpp"

vtb::uart::Port<10> uart_port(huart1);
vtb::can::Port can_port(hcan1);

class MotorPlot : public vtb::CycleTask<1024> {
 public:
  explicit MotorPlot(uint32_t delay, const char* name)
      : CycleTask<1024>(delay, name),
        uart_sender_(uart_port),
        uart_receiver_(
            uart_port,
            vtb::Callback::bind<MotorPlot, &MotorPlot::callback>(this)),
        mgroup_(can_port),
        motor_(mgroup_) {}

  void update() override {
    switch_buffer_.interface().header = 0xAB;
    switch_buffer_.interface().angle = motor_.get_angle();
    switch_buffer_.interface().current = motor_.get_current();
    switch_buffer_.interface().speed = motor_.get_speed();
    switch_buffer_.interface().temperature = motor_.get_temperature();
    uart_sender_.send(switch_buffer_.data(), switch_buffer_.size);
  }

  void callback(const vtb::RcvData& rcv) {}

 private:
  vtb::uart::Sender uart_sender_;
  vtb::uart::Receiver uart_receiver_;
  vtb::M3508Group mgroup_;
  vtb::MotorM3508<2> motor_;

  VTB_PACK t {
    uint8_t header;
    float angle;
    float speed;
    float current;
    float temperature;
  };
  vtb::SwitchBuffer<t> switch_buffer_;
};

MotorPlot motor_plot(1, "plot");