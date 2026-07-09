#ifndef VTB_PID
#define VTB_PID

#include <cmath>
#include <utility>
#include <vector>

#include "vertebra/math/function.hpp"

namespace vtb
{

class PidCtrl
{
public:
  struct State
  {
    float real = 0;
    float prev_real = 0;
    float prev_err = 0;
    float itg_err = 0;
    float itg_delta = 0;
    float target = 0;
    float dt = 0;
  };

  explicit PidCtrl(float kp, float ki, float kd);

  PidCtrl & set_kp(float kp);
  PidCtrl & set_ki(float ki);
  PidCtrl & set_kd(float kd);

  PidCtrl & set_pre(void (*pre)(State &));
  PidCtrl & set_post(void (*post)(State &));

  PidCtrl & set_term_p(float (*term_p)(const State &));
  PidCtrl & set_term_i(float (*term_i)(const State &));
  PidCtrl & set_term_d(float (*term_d)(const State &));

  PidCtrl & add_term(float k, float (*term)(const State &));

  float instruct(float real, float target, float dt);

  static float integral_rect(const State & state);

  static float _integral_detach(const State & state, float threshold);
  static float _integral_vary(const State & state, float begin, float end);
  static float _integral_trapezoid(const State & state);

  static float term_p_plain(const State & state);
  static float term_i_plain(const State & state);
  static float term_d_plain(const State & state);

  static float term_d_measure(const State & state);

  static void _deadzone_detach(State & state, float deadzone);
  static void _integral_limit(State & state, float limit);

private:
  void update_state(float real, float target, float dt);

  float kp_, ki_, kd_;
  void (*pre_)(State &) = nullptr;
  float (*integral_)(const State &) = integral_rect;
  float (*term_p_)(const State &) = term_p_plain;
  float (*term_i_)(const State &) = term_i_plain;
  float (*term_d_)(const State &) = term_d_plain;
  float (*post_)(float) = nullptr;
  std::vector<std::pair<float, float (*)(const State &)>> attach_;
  State state_;
};
}  // namespace vtb

#define VTB_PID_PRE_DEADZONE_DETACH(deadzone) \
  [](vtb::PidCtrl::State & s) -> void { vtb::PidCtrl::_deadzone_detach(s, (deadzone)); }

#define VTB_PID_PRE_INTEGRAL_LIMIT(lim) \
  [](vtb::PidCtrl::State & s) -> void { vtb::PidCtrl::_integral_limit(s, (lim)); }

#define VTB_PID_INTEGRAL_DETACH(threshold) \
  [](const vtb::PidCtrl::State & s) -> float { return vtb::PidCtrl::_integral_detach(s, (threshold)); }

#define VTB_PID_INTEGRAL_VARY(begin, end) \
  [](const vtb::PidCtrl::State & s) -> float { return vtb::PidCtrl::_integral_vary(s, (begin), (end)); }

#define VTB_PID_INTEGRAL_TRAPEZOID \
  [](const vtb::PidCtrl::State & s) -> float { return vtb::PidCtrl::_integral_trapezoid(s); }

#define VTB_PID_P_PLAIN vtb::PidCtrl::term_p_plain
#define VTB_PID_I_PLAIN vtb::PidCtrl::term_i_plain
#define VTB_PID_D_PLAIN vtb::PidCtrl::term_d_plain

#define VTB_PID_D_MEASURE vtb::PidCtrl::term_d_measure

#define VTB_PID_POST_LIMIT(lim) \
  [](float var) -> float { return vtb::clamp(var, -std::abs(lim), std::abs(lim)); }

#endif