#ifndef VTB_CYCLE_TASK
#define VTB_CYCLE_TASK

#include "vtb_task.hpp"
#include "main.h"

namespace vtb {

template<uint32_t SIZE>
class CycleTask : public Task<SIZE>
{
public:
  explicit CycleTask(uint32_t delay, const char * name = nullptr, Priority priority = Priority::None);
  virtual ~CycleTask() = default;

  virtual update() = 0;
  
private:
  void run() override;

  uint32_t delay_;
};

}

#endif