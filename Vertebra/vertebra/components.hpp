#ifndef VTB_COMPONENTS
#define VTB_COMPONENTS

#include <algorithm>

#include "FreeRTOS.h"

namespace vtb
{
#if (configMAX_PRIORITIES >= 56)
enum class Priority : uint32_t
{
  // clang-format off
  None          = 0,
  Idle          = 1,
  Low           = 8,
  Low1          = 8 + 1,
  Low2          = 8 + 2,
  Low3          = 8 + 3,
  Low4          = 8 + 4,
  Low5          = 8 + 5,
  Low6          = 8 + 6,
  Low7          = 8 + 7,
  BelowNormal   = 16,
  BelowNormal1  = 16 + 1,
  BelowNormal2  = 16 + 2,
  BelowNormal3  = 16 + 3,
  BelowNormal4  = 16 + 4,
  BelowNormal5  = 16 + 5,
  BelowNormal6  = 16 + 6,
  BelowNormal7  = 16 + 7,
  Normal        = 24,
  Normal1       = 24 + 1,
  Normal2       = 24 + 2,
  Normal3       = 24 + 3,
  Normal4       = 24 + 4,
  Normal5       = 24 + 5,
  Normal6       = 24 + 6,
  Normal7       = 24 + 7,
  AboveNormal   = 32,
  AboveNormal1  = 32 + 1,
  AboveNormal2  = 32 + 2,
  AboveNormal3  = 32 + 3,
  AboveNormal4  = 32 + 4,
  AboveNormal5  = 32 + 5,
  AboveNormal6  = 32 + 6,
  AboveNormal7  = 32 + 7,
  High          = 40,
  High1         = 40 + 1,
  High2         = 40 + 2,
  High3         = 40 + 3,
  High4         = 40 + 4,
  High5         = 40 + 5,
  High6         = 40 + 6,
  High7         = 40 + 7,
  Realtime      = 48,
  Realtime1     = 48 + 1,
  Realtime2     = 48 + 2,
  Realtime3     = 48 + 3,
  Realtime4     = 48 + 4,
  Realtime5     = 48 + 5,
  Realtime6     = 48 + 6,
  Realtime7     = 48 + 7,
  ISR = 56
  // clang-format on
};
#elif (configMAX_PRIORITIES >= 7)
enum class Priority : uint32_t
{
  // clang-format off
  None          = 0,
  Idle          = 1,
  Low           = 2,
  BelowNormal   = 3,
  Normal        = 4,
  AboveNormal   = 5,
  High          = 6,
  Realtime      = 7
  // clang-format on
};
#else
#error Vertebra priority not supported.
#endif

void delay(uint32_t millisec);

template <class T>
constexpr const T& clamp( const T& val, const T& lo, const T& hi ) {
    return std::max( lo, std::min( val, hi ) );
}
}  // namespace vtb

#endif