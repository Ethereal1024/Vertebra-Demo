#include "cycle_task.hpp"

namespace vtb
{

template <uint32_t SIZE>
CycleTask<SIZE>::CycleTask(uint32_t delay, const char * name, Priority priority)
: Task<SIZE>(name, priority), delay_(delay) {}

template <uint32_t SIZE>
void CycleTask<SIZE>::run() {
  for(;;) {
    update();
    delay(delay_);
  }
}

}  // namespace vtb