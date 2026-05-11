#include "vtb_task.hpp"

namespace vtb
{

void TaskLauncher::register_tasks(const Task * task) { tasks_.emplace_back(task); }

void TaskLauncher::launch_tasks()
{
  for (auto task : tasks_) task->start();
}

Task::Task(std::string name, uint16_t stackDepth, UBaseType_t priority)
: name_(name), stack_depth_(stackDepth), priority_(priority)
{
  TaskLauncher::instance().register_tasks(this);
}

Task::~Task()
{
  if (handle_ != nullptr) {
    vTaskDelete(handle_);
    handle_ = nullptr;
  }
}

void Task::start()
{
  BaseType_t result =
    xTaskCreate(task_entry, name_.c_str(), stack_depth_, this, priority_, &handle_);
  if (result != pdPASS) {
    Error_Handler();
  }
}

void Task::task_entry(void * argument)
{
  auto * task = static_cast<Task *>(argument);
  task->run();
}

}  // namespace vtb
