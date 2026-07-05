#include "callback.hpp"

namespace vtb
{

Callback::Callback(void (*callback)(const RcvData &)) : cbarg_(callback) {}

Callback::Callback(void * ctx, void (*callback)(void *, const RcvData &))
: ctx_(ctx), cbctx_(callback)
{
}

Callback::Callback(void (*callback)()) : cbnarg_(callback) {}

void Callback::call(const RcvData & data) const
{
  if (ctx_ && cbctx_) cbctx_(ctx_, data);
  else if (cbarg_) cbarg_(data);
  else cbnarg_();
}

void Callback::call() const
{
  RcvData empty;
  call(empty);
}

Callback::operator bool() const { return (ctx_ && cbctx_) || cbarg_ || cbnarg_; }

}  // namespace vtb