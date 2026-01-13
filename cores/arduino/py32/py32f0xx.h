#pragma once

/*
 * This core historically shipped its own "py32f0xx.h" which ends up earlier
 * on the include path than the CMSIS umbrella header of the same name.
 *
 * That shadowing breaks HAL/LL compilation because the CMSIS umbrella header
 * is where device selection (and many memory size macros) are defined.
 *
 * Solution: delegate to the next "py32f0xx.h" on the include path.
 */
#if defined(__GNUC__)
  #include_next "py32f0xx.h"
#else
  #error "This core requires a compiler supporting #include_next (GCC/Clang)."
#endif
