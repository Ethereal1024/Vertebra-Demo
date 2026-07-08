#ifndef VTB_DEFS
#define VTB_DEFS

#include "main.h"

#ifdef PI
#undef PI
#endif
constexpr float PI = 3.14159265358979323846f;

#if defined(STM32F0) || defined(STM32F1) || defined(STM32F3) || defined(STM32G0) || \
  defined(STM32G4) || defined(STM32L0) || defined(STM32L1)
#define VTB_DMA_CHANNEL
#elif defined(STM32F2) || defined(STM32F4) || defined(STM32F7) || defined(STM32L4) || \
  defined(STM32H7) || defined(STM32L4PLUS)
#define VTB_DMA_STREAM
#endif

#define VTB_PACK struct  __attribute__((packed))

#endif