#ifndef VTB_SWITCH_BUFFER
#define VTB_SWITCH_BUFFER

#include <cstdint>

#include "main.h"

namespace vtb
{

template <typename T>
class SwitchBuffer
{
public:
  static constexpr size_t size = sizeof(T);
  T & interface() { return buffer_.interface; }
  uint8_t* data() { return &buffer_.data[0]; }

private:
  union __buffer {
    uint8_t data[size];
    T interface;
  } buffer_;
};

}  // namespace vtb

#endif