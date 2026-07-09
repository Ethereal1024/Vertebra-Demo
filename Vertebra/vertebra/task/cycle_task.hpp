#ifndef VTB_CYCLE_TASK
#define VTB_CYCLE_TASK

#include "main.h"
#include "vtb_task.hpp"

namespace vtb
{

template <uint32_t SIZE>
class CycleTask : public Task<SIZE>
{
public:
  explicit CycleTask(
    uint32_t delay, const char * name = nullptr, Priority priority = Priority::None)
  : Task<SIZE>(name, priority), delay_(delay)
  {
  }

  virtual ~CycleTask() = default;

protected:
  virtual void update() = 0;
  virtual void start() {}
  uint32_t get_delay() { return delay_; }

private:
  void run() override
  {
    start();
    for (;;) {
      update();
      delay(delay_);
    }
  }

  uint32_t delay_;
};

}  // namespace vtb

#endif