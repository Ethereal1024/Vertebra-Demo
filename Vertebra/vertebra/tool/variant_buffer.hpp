#ifndef VTB_VARIANT_BUFFER
#define VTB_VARIANT_BUFFER

#include <cstdint>
#include <cstring>

#include "main.h"

namespace vtb {

template <typename... T>
struct BufferSize;

template <typename First, typename... Rest>
struct BufferSize<First, Rest...> {
  static const size_t value = sizeof(First) + BufferSize<Rest...>::value;
};

template <>
struct BufferSize<> {
  static const size_t value = 0;
};

template <typename... Args>
class VariantBuffer {
  static_assert(sizeof...(Args) != 0,
                "VariantBuffer should contains at least one element.");

 public:
  explicit VariantBuffer(Args*... args) : item_ptrs_{args...} {}

  static constexpr size_t size_ = BufferSize<Args...>::value;
  static constexpr size_t num_ = sizeof...(Args);

  void flush_in() {
    size_t idx = 0;
    for (size_t i = 0; i < num_; i++) {
      memcpy(&buffer_[idx], item_ptrs_[i], item_sizes_[i]);
      idx += item_sizes_[i];
    }
  }

  void flush_out() {
    size_t idx = 0;
    for (size_t i = 0; i < num_; i++) {
      memcpy(item_ptrs_[i], &buffer_[idx], item_sizes_[i]);
      idx += item_sizes_[i];
    }
  }

  uint8_t* data() { return &buffer_[0]; }

 private:
  uint8_t buffer_[BufferSize<Args...>::value] = {0};
  size_t item_sizes_[num_] = {sizeof(Args)...};
  void* item_ptrs_[num_];
};

}  // namespace vtb

#endif