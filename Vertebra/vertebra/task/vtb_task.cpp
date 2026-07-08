#include "vtb_task.hpp"

#include "portmacro.h"

namespace vtb {

void TaskLauncher::register_task(TaskBase* task) { tasks_.emplace_back(task); }

void TaskLauncher::register_loon(Loon* loon) { loons_.emplace_back(loon); }

void TaskLauncher::launch_tasks() {
  for (auto loon : loons_) loon->awake();
  for (auto task : tasks_) task->setup();
}

}  // namespace vtb
