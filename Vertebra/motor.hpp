#ifndef VTB_MOTOR
#define VTB_MOTOR

class Motor
{
public:
  virtual ~Motor() = default;
  virtual void force(float strength) = 0;
  virtual float get_speed() = 0;
  virtual float get_angle() = 0;
};

#endif