#ifndef VTB_TASK
#define VTB_TASK

#include <cstdint>
#include <vector>

#include "FreeRTOS.h"
#include "design/singleton.hpp"
#include "components.hpp"
#include "main.h"
#include "task.h"

namespace vtb
{
class TaskBase
{
public:
  virtual void start() = 0;
};

class TaskLauncher : public Singleton<TaskLauncher>
{
public:
  friend class Singleton<TaskLauncher>;

  void register_tasks(const TaskBase * task);
  void launch_tasks();

private:
  TaskLauncher() = default;

  std::vector<TaskBase *> tasks_;
};

template <uint32_t SIZE>
class Task : public TaskBase
{
  static_assert(SIZE % sizeof(StackType_t) == 0, "SIZE must be multiple of stack word size");

public:
  explicit Task(const char * name = nullptr, Priority priority = Priority::None);
  virtual ~Task();

  void start() override;

protected:
  virtual void run() = 0;

private:
  static void task_entry(void * argument);
  static constexpr uint32_t STACK_DEPTH = SIZE / sizeof(StackType_t);

  const char * name_;
  Priority priority_;

  StackType_t stack_[STACK_DEPTH];
  StaticTask_t tcb_;
  TaskHandle_t handle_ = nullptr;
};

}  // namespace vtb
#endif