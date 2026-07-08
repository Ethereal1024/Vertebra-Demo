#ifndef MOTOR_M3508
#define MOTOR_M3508

#include "main.h"

#ifdef HAL_CAN_MODULE_ENABLED

#include <math.h>
#include <sys/types.h>

#include <cmath>
#include <memory>

#include "vertebra/components.hpp"
#include "vertebra/connection/can_port.hpp"
#include "vertebra/connection/can_receiver.hpp"
#include "vertebra/connection/can_sender.hpp"
#include "vertebra/defs.hpp"
#include "vertebra/device/motor/motor.hpp"

namespace vtb {
class M3508Group {
 public:
  explicit M3508Group(can::Port& port)
      : port_(port), sender0_(port), sender1_(port) {
    sender0_.set_frame_id(0x200)
        .set_frame_type(can::FrameType::Standard)
        .set_remote_type(can::RemoteType::Data)
        .set_data_len(8);
    sender1_.set_frame_id(0x1FF)
        .set_frame_type(can::FrameType::Standard)
        .set_remote_type(can::RemoteType::Data)
        .set_data_len(8);
  }

  template <uint16_t ID>
  can::Receiver create_receiver(
      const Callback callback) {
    uint32_t frame_id = 0x200 | ID;
    return can::Receiver(port_, frame_id, callback);
  }

  template <uint16_t ID>
  void force(float strength) {
    constexpr float raw = 16384.0f;
    constexpr float amp = 20.0f;
    constexpr float scale = raw / amp;

    float input = clamp(strength * scale, -raw, raw);
    strenghths_[ID - 1] = static_cast<int16_t>(std::lround(input));
    if (ID <= 4)
      send(false);
    else
      send(true);
  }

 private:
  void send(bool greater) {
    uint8_t data[8];
    int offset = 0;
    can::Sender* sender = &sender0_;
    if (greater) {
      offset = 4;
      sender = &sender1_;
    }
    for (size_t i = 0; i < 4; i++) {
      size_t j = i * 2;
      size_t k = i + offset;
      data[j] = strenghths_[k] >> 8;
      data[j + 1] = strenghths_[k];
    }
    sender->send(data);
  }

  can::Port& port_;

  can::Sender sender0_;
  can::Sender sender1_;

  int16_t strenghths_[8];
};

template <uint16_t ID>
class MotorM3508 : public Motor {
 public:
  explicit MotorM3508(M3508Group& group)
      : group_(group),
        receiver_(group.create_receiver<ID>(
            Callback::bind<MotorM3508, &MotorM3508::callback>(this))) {
    static_assert(ID >= 1 && ID <= 8,
                  "M3508 Motor ID must be the int between 1 and 8");
  }

  void force(float strenghth) override { group_.force<ID>(strenghth); }

  float get_angle() override {
    constexpr float raw = 8191.0f;
    constexpr float scale = 2 * PI / raw;
    constexpr float offset = -PI;
    return (angle_ * scale + offset) * reduc_ratio_inv_;
  }

  float get_speed() override {
    constexpr float scale = 2 * PI / 60.0f;
    return speed_ * scale * reduc_ratio_inv_;
  }

  float get_current() { return (float)current_ / 1000.0f; }

  float get_temperature() { return temperature_; }

  void set_reduc_ratio(float ratio) { reduc_ratio_inv_ = 1.0f / ratio; }

  void callback(const RcvData& rcv) {
    angle_ = ((rcv.data[0] << 8) | rcv.data[1]);
    speed_ = ((rcv.data[2] << 8) | rcv.data[3]);
    current_ = ((rcv.data[4] << 8) | rcv.data[5]);
    temperature_ = rcv.data[6];
  }

 private:
  M3508Group& group_;
  can::Receiver receiver_;

  float reduc_ratio_inv_ = 187.0f / 3591.0f;

  int16_t angle_;
  int16_t speed_;
  int16_t current_;
  int8_t temperature_;
};

}  // namespace vtb

#endif

#endif