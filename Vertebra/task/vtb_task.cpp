#include "vtb_task.hpp"
#include "portmacro.h"

namespace vtb
{

void TaskLauncher::register_tasks(const TaskBase * task) { tasks_.emplace_back(task); }

void TaskLauncher::launch_tasks()
{
  for (auto task : tasks_) task->start();
}

template <uint32_t SIZE>
Task<SIZE>::Task(const char * name, Priority priority) : name_(name), priority_(priority)
{
  if (priority_ == Priority::None) priority_ = Priority::Normal;
  TaskLauncher::instance().register_tasks(this);
}

template <uint32_t SIZE>
Task<SIZE>::~Task()
{
  if (handle_ != nullptr) {
    vTaskDelete(handle_);
    handle_ = nullptr;
  }
}

template <uint32_t SIZE>
void Task<SIZE>::start()
{
  char empty = '\0';
  const char* name = name_ ? name_ : &empty;
  // clang-format off
  handle_ = xTaskCreateStatic(
    task_entry, 
    name, 
    STACK_DEPTH, 
    this, 
    static_cast<UBaseType_t>(priority_) - 1U, 
    stack_, 
    &tcb_
  );
  // clang-format on
}

template <uint32_t SIZE>
void Task<SIZE>::task_entry(void * argument)
{
  auto * task = static_cast<Task<SIZE> *>(argument);
  task->run();
}

}  // namespace vtb
