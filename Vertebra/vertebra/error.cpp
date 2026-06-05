#include "error.hpp"

namespace vtb
{

void Error::register_error_callback(uint8_t id, Callback callback)
{
  instance().callbacks_[id].emplace_back(callback);
}

void Error::handle_error(uint8_t id, RcvData rcv)
{
  auto it = instance().callbacks_.find(id);
  if (it != instance().callbacks_.end()) {
    for (const auto & cb : it->second) {
      cb.call(rcv);
    }
  }
}

void Error::handle_error(uint8_t id)
{
  auto it = instance().callbacks_.find(id);
  if (it != instance().callbacks_.end()) {
    for (const auto & cb : it->second) {
      cb.call();
    }
  }
}

void Error::handle_error(RcvData rcv)
{
  for (const auto& [_, group] : instance().callbacks_) {
    for (const auto & cb : group) {
      cb.call(rcv);
    }
  }
}

void Error::handle_error()
{
  for (const auto& [_, group] : instance().callbacks_) {
    for (const auto & cb : group) {
      cb.call();
    }
  }
}

}  // namespace vtb