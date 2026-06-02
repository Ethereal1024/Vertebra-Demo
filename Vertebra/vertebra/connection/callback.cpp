#include "callback.hpp"

namespace vtb
{

Callback::Callback(void (*callback)(const RcvData &)) : ctx_(callback)
{
  cb_ = [](void * ctx, const RcvData & data) {
    auto cb = static_cast<void (*)(const RcvData &)>(ctx);
    cb(data);
  };
}

Callback::Callback(void * ctx, void (*callback)(void *, const RcvData &)) : ctx_(ctx), cb_(callback)
{
}

Callback::Callback(void (*callback)()) : ctx_(callback)
{
  cb_ = [](void * ctx, const RcvData & data) {
    auto cb = static_cast<void (*)()>(ctx);
    cb();
  };
}

void Callback::call(const RcvData & data) const
{
  if (!cb_ || !ctx_) return;
  cb_(ctx_, data);
}

void Callback::call() const
{
  RcvData empty;
  call(empty);
}

Callback::operator bool() const { return ctx_ && cb_; }

}  // namespace vtb