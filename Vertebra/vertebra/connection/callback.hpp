#ifndef VTB_CALLBACK
#define VTB_CALLBACK

#include <cstddef>

namespace vtb
{

struct RcvData
{
  const uint8_t * data;
  size_t size;
};

class Callback
{
public:
  Callback(void (*callback)(const RcvData &));
  Callback(void * ctx, void (*lambda_wr)(void *, const RcvData &));
  void call(const RcvData &);

private:
  void * ctx_ = nullptr;
  void (*callback_)(const RcvData &) = nullptr;
  void (*lambda_wr_)(void *, const RcvData &) = nullptr;
};

}  // namespace vtb

#endif