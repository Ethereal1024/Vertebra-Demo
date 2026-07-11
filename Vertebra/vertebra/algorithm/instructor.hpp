#ifndef VTB_CONTROLLER
#define VTB_CONTROLLER

#include <vector>
#include <utility>

namespace vtb
{

template <typename T>
class Instructor
{
public:
  using StateType = typename T::State;

  virtual void update_state(float actual, float logical, float dt) = 0;

  float instruct(float actual, float logical, float dt)
  {
    update_state(actual, logical, dt);
    if (pre_ != nullptr) pre_(state_);
    float result = 0;
    for (auto [k, fn] : terms_) {
      result += k * fn(state_);
    }
    if (post_ != nullptr) result = post_(result);
    return result;
  }

  void reset() { state_ = StateType(); }

  T & set_pre(void (*pre)(StateType &))
  {
    pre_ = pre;
    return *this;
  }
  T & set_post(float (*post)(float))
  {
    post_ = post;
    return *this;
  }

  T & set_error_fn(float (*error_fn)(float actual, float logical))
  {
    error_fn_ = error_fn;
    return *this;
  }

  T & set_k(float k, size_t idx)
  {
    if (idx < terms_.size()) terms_[idx].first = k;
    return *this;
  }

  T & add_term(float k, float (*term)(const StateType &))
  {
    terms_.push_back(std::pair<float, float (*)(const StateType &)>(k, term));
    return *this;
  }

private:
  StateType state_;
  void (*pre_)(StateType &) = nullptr;
  float (*error_fn_)(float target, float real) = linear_diff;
  float (*post_)(float) = nullptr;
  std::vector<std::pair<float, float (*)(const StateType &)>> terms_;
};

}  // namespace vtb

#endif