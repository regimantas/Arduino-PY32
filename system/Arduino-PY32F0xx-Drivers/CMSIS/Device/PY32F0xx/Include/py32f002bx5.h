/**
  ******************************************************************************
  * @file    py32f002bx5.h
  * @brief   CMSIS device header for PY32F002Bx5.
  *
  * Notes:
  * - PY32F002B is register-compatible with PY32F002A in this core.
  * - Flash size differs; this wrapper reuses the PY32F002Ax5 definitions and
  *   overrides the FLASH size-related macros for the 24KB main flash part.
  ******************************************************************************
  */

#ifndef __PY32F002BX5_H
#define __PY32F002BX5_H

#include "py32f002ax5.h"

/* Override memory size macros for PY32F002Bx5 (24KB main flash). */
#ifdef FLASH_END
#undef FLASH_END
#endif
#define FLASH_END             (0x08005FFFUL)  /*!< FLASH end address (24KB main flash) */

#ifdef FLASH_SIZE
#undef FLASH_SIZE
#endif
#define FLASH_SIZE            (FLASH_END - FLASH_BASE + 1)

#ifdef FLASH_PAGE_NB
#undef FLASH_PAGE_NB
#endif
#define FLASH_PAGE_NB         (FLASH_SIZE / FLASH_PAGE_SIZE)

#ifdef FLASH_SECTOR_NB
#undef FLASH_SECTOR_NB
#endif
#define FLASH_SECTOR_NB       (FLASH_SIZE / FLASH_SECTOR_SIZE)

#endif /* __PY32F002BX5_H */
