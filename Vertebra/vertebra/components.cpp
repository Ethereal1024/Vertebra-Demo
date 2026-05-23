#include "components.hpp"

#include "FreeRTOS.h"
#include "task.h"
#include <algorithm>

namespace vtb
{
void delay(uint32_t millisec)
{
  static_assert(INCLUDE_vTaskDelay, "FreeRTOS Delay not configured");
  TickType_t ticks = millisec / portTICK_PERIOD_MS;
  vTaskDelay(ticks ? ticks : 1);
}

}  // namespace vtb