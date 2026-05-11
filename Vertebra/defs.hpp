#ifndef VTB_DEFS
#define VTB_DEFS

#include "main.h"

#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif

#ifdef PI
  #undef PI
#endif
constexpr float PI = 3.14159265358979323846f;

#endif