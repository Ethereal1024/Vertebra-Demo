#ifndef VTB_MOTOR
#define VTB_MOTOR

class MotorBase {
 public:
  virtual void force(float strength) = 0;
};

#endif