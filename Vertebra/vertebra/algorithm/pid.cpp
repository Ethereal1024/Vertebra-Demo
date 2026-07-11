#include "pid.hpp"

#include "vertebra/components.hpp"

namespace vtb
{

PidCtrl::PidCtrl(float kp, float ki, float kd) : kp_(kp), ki_(ki), kd_(kd) {}

PidCtrl & PidCtrl::set_kp(float kp)
{
  kp_ = kp;
  return *this;
}
PidCtrl & PidCtrl::set_ki(float ki)
{
  ki_ = ki;
  return *this;
}
PidCtrl & PidCtrl::set_kd(float kd)
{
  kp_ = kd;
  return *this;
}

PidCtrl & PidCtrl::set_pre(void (*pre)(State &))
{
  pre_ = pre;
  return *this;
}
PidCtrl & PidCtrl::set_post(float (*post)(float))
{
  post_ = post;
  return *this;
}

PidCtrl & PidCtrl::set_integral(float (*integral)(const State & state))
{
  integral_ = integral;
  return *this;
}

PidCtrl & PidCtrl::set_term_p(float (*term_p)(const State &))
{
  term_p_ = term_p;
  return *this;
}
PidCtrl & PidCtrl::set_term_i(float (*term_i)(const State &))
{
  term_p_ = term_i;
  return *this;
}
PidCtrl & PidCtrl::set_term_d(float (*term_d)(const State &))
{
  term_p_ = term_d;
  return *this;
}

PidCtrl & PidCtrl::add_term(float k, float (*term)(const State &))
{
  attach_.push_back(std::pair<float, float (*)(const State &)>(k, term));
  return *this;
}

void PidCtrl::reset() { state_ = State(); }

float PidCtrl::instruct(float real, float target, float dt)
{
  update_state(real, target, dt);
  if (pre_ != nullptr) pre_(state_);
  float p = term_p_(state_);
  float i = term_i_(state_);
  float d = term_d_(state_);
  float atta(0);
  for (auto [k, fn] : attach_) {
    atta += k * fn(state_);
  }
  float result = kp_ * p + ki_ * i + kd_ * d + atta;
  if (post_ != nullptr) result = post_(result);
  return result;
}

float PidCtrl::integral_rect(const State & state) { return (state.target - state.real) * state.dt; }

float PidCtrl::term_p_plain(const State & state) { return state.target - state.real; }

float PidCtrl::term_i_plain(const State & state) { return state.itg_err + state.itg_delta; }

float PidCtrl::term_d_plain(const State & state)
{
  if (state.dt == 0) return 0;
  return (state.target - state.real - state.prev_err) / state.dt;
}

float PidCtrl::term_d_measure(const State & state)
{
  if (state.dt == 0) return 0;
  return (state.prev_real - state.real) / state.dt;
}

void PidCtrl::_deadzone_detach(State & state, float deadzone)
{
  float err = state.target - state.real;
  if (std::abs(err) < deadzone) {
    state.itg_delta = 0.0f;
    state.target = state.real;
    state.prev_err = 0.0f;
  }
}

void PidCtrl::_integral_limit(State & state, float limit)
{
  state.itg_delta =
    vtb::clamp(state.itg_err + state.itg_delta, -std::abs(limit), std::abs(limit)) - state.itg_err;
}

void PidCtrl::_angular_fix(State & state)
{
  if (state.target - state.real > vtb::PI) state.target -= 2 * vtb::PI;
}

float PidCtrl::_integral_detach(const State & state, float threshold)
{
  float err = state.target - state.real;
  if (std::abs(err) > threshold) return 0.0f;
  return err * state.dt;
}

float PidCtrl::_integral_vary(const State & state, float begin, float end)
{
  if (end <= begin) return _integral_detach(state, begin);
  float err = state.target - state.real;
  float err_abs = std::abs(err);
  if (err_abs < begin) return err * state.dt;
  if (err_abs > end) return 0.0f;
  return err * state.dt * (end - err_abs) / (end - begin);
}

float PidCtrl::_integral_trapezoid(const State & state)
{
  return (state.prev_err + state.target - state.real) * state.dt * 0.5;
}

void PidCtrl::update_state(float real, float target, float dt)
{
  state_.prev_real = state_.real;
  state_.prev_err = state_.target - state_.real;

  state_.real = real;
  state_.target = target;
  state_.dt = dt;

  state_.itg_err += state_.itg_delta;
  state_.itg_delta = integral_(state_);
}

}  // namespace vtb