#ifndef __MOTOR__
#define __MOTOR__

class MotorBase {
 public:
  virtual void force(float strength) = 0;
};

#endif