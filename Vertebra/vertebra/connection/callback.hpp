#ifndef VTB_CALLBACK
#define VTB_CALLBACK

#include <cstdint>

#include "main.h"

namespace vtb
{

struct RcvData
{
  const uint8_t * data = nullptr;
  size_t size = 0;
  bool sticky = false;
};

class Callback
{
public:
  Callback() = default;
  Callback(void (*callback)(const RcvData &));
  Callback(void (*callback)());
  void call(const RcvData &) const;
  void call() const;

  explicit operator bool() const;

  template <typename T, void (T::*method)(const RcvData &)>
  static Callback bind(void * obj)
  {
    return Callback(
      obj, [](void * ptr, const RcvData & data) { (static_cast<T *>(ptr)->*method)(data); });
  }

  template <typename T, void (T::*method)()>
  static Callback bind(void * obj)
  {
    return Callback(
      obj, [](void * ptr, const RcvData & data) { (static_cast<T *>(ptr)->*method)(); });
  }

private:
  Callback(void * ctx, void (*callback)(void *, const RcvData &));

  void * ctx_ = nullptr;
  void (*cbarg_)(const RcvData &) = nullptr;
  void (*cbnarg_)() = nullptr;
  void (*cbctx_)(void *, const RcvData &) = nullptr;
};

}  // namespace vtb

#endif