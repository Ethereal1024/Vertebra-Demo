#include "vtb_task.hpp"
#include "portmacro.h"

namespace vtb
{

void TaskLauncher::register_tasks(TaskBase * task) { tasks_.emplace_back(task); }

void TaskLauncher::launch_tasks()
{
  for (auto task : tasks_) task->start();
}

}  // namespace vtb
