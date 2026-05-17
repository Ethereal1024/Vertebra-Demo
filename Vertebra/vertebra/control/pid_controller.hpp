#ifndef VTB_PID_CONTROLLER
#define VTB_PID_CONTROLLER

namespace vtb {

template <typename T>
class PidController {
 public:
  explicit PidController(float kp, float ki, float kd)
      : kp_(kp), ki_(ki), kd_(kd) {}

 private:
  const T* feedback_;
  T output_;
  float kp_ = 0.0f, ki_ = 0.0f, kd_ = 0.0f;
  struct {
    float time = -1.0f;
    float error = 0.0f;
  } prev_;
  struct {
    float integral = -1.0f;
    float output = -1.0f;
  } limit_;
};

}  // namespace vtb

#endif

/*
auto controller = PidController(a).attach_pid(p, i, d).attach_pid(p, i, d)

*/