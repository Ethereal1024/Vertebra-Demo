#include "vtb_bridge.h"
#include "task/vtb_task.hpp"

extern "C" {
void vtb_launch_tasks() {
  vtb::TaskLauncher::instance().launch_tasks();
}
}