#ifndef VTB_FUNCTION
#define VTB_FUNCTION

namespace vtb {

template <class T>
T clamp(const T & val, const T & lo, const T & hi)
{
  return std::max(lo, std::min(val, hi));
}

template <class T>
T abs(const T & val)
{
  using std::abs;
  return abs(val);
}

}

#endif