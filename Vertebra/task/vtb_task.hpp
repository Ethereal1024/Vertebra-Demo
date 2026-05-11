#ifndef VTB_TASK
#define VTB_TASK

#include <string>
#include <vector>

#include "FreeRTOS.h"
#include "design/singleton.hpp"
#include "main.h"
#include "task.h"

namespace vtb
{

class Task;

class TaskLauncher : public Singleton<TaskLauncher>
{
public:
  friend class Singleton<TaskLauncher>;

  void register_tasks(const Task * task);
  void launch_tasks();

private:
  TaskLauncher() = default;

  std::vector<Task *> tasks_;
};

class Task
{
public:
  explicit Task(std::string name, uint16_t stackDepth, UBaseType_t priority);
  virtual ~Task();

  void start();

protected:
  virtual void run() = 0;

private:
  static void task_entry(void * argument);

  std::string name_;
  uint16_t stack_depth_;
  UBaseType_t priority_;
  TaskHandle_t handle_ = nullptr;
};

}  // namespace vtb
#endif