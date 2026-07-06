#ifndef VTB_TASK
#define VTB_TASK

#include <vector>
#include <string>

#include "FreeRTOS.h"
#include "main.h"
#include "task.h"
#include "vertebra/components.hpp"
#include "vertebra/design/singleton.hpp"
#include "vertebra/design/loon.hpp"

namespace vtb {
class TaskBase {
 public:
  virtual void start() = 0;
  virtual ~TaskBase() = default;
};

class TaskLauncher : public Singleton<TaskLauncher> {
 public:
  friend class Singleton<TaskLauncher>;

  void register_task(TaskBase* task);
  void register_loon(Loon* loon);
  void launch_tasks();

 private:
  TaskLauncher() = default;

  std::vector<TaskBase*> tasks_;
  std::vector<Loon*> loons_;
};

template <uint32_t SIZE>
class Task : public TaskBase {
  static_assert(SIZE % sizeof(StackType_t) == 0,
                "SIZE must be multiple of stack word size");

 public:
  explicit Task(const char* name = nullptr, Priority priority = Priority::None)
      : name_(name), priority_(priority) {
    if (priority_ == Priority::None) priority_ = Priority::Normal;
    TaskLauncher::instance().register_task(this);
  }

  virtual ~Task() {
    if (handle_ != nullptr) {
      vTaskDelete(handle_);
      handle_ = nullptr;
    }
  }

  void start() override {
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

  std::string get_name() {
    if (name_ == nullptr) return "";
    return name_;
  }

  Priority get_priority() { return priority_; }

 protected:
  virtual void run() = 0;

 private:
  static void task_entry(void* argument) {
    auto* task = static_cast<Task<SIZE>*>(argument);
    task->run();
  }
  
  static constexpr uint32_t STACK_DEPTH = SIZE / sizeof(StackType_t);

  const char* name_;
  Priority priority_;

  StackType_t stack_[STACK_DEPTH];
  StaticTask_t tcb_;
  TaskHandle_t handle_ = nullptr;
};

}  // namespace vtb
#endif