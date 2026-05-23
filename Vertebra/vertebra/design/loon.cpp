#include "loon.hpp"
#include "vertebra/task/vtb_task.hpp"

namespace vtb {

Loon::Loon() { TaskLauncher::instance().register_loon(this); }

}  // namespace vtb
