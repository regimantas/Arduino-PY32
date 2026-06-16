#include "board.h"

#ifndef PY32_BOR_CONFIG
#define PY32_BOR_CONFIG -1
#endif

#ifndef PY32_APPLY_BOR_OPTION_BYTES
#define PY32_APPLY_BOR_OPTION_BYTES 0
#endif

#if defined(PY32F0xx) && defined(FLASH_OPTR_BOR_EN) && defined(FLASH_OPTR_BOR_LEV) && PY32_APPLY_BOR_OPTION_BYTES
#include "py32f0xx_hal_flash.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void __libc_init_array(void);

#if defined(PY32F0xx) && defined(FLASH_OPTR_BOR_EN) && defined(FLASH_OPTR_BOR_LEV) && (PY32_BOR_CONFIG >= 0) && PY32_APPLY_BOR_OPTION_BYTES
static void py32_apply_bor_option_bytes(void)
{
  uint32_t borConfig;

#if PY32_BOR_CONFIG == 0
  borConfig = OB_BOR_DISABLE;
#elif PY32_BOR_CONFIG == 1
  borConfig = OB_BOR_ENABLE | OB_BOR_LEVEL_1p7_1p8;
#elif PY32_BOR_CONFIG == 2
  borConfig = OB_BOR_ENABLE | OB_BOR_LEVEL_1p9_2p0;
#elif PY32_BOR_CONFIG == 3
  borConfig = OB_BOR_ENABLE | OB_BOR_LEVEL_2p1_2p2;
#elif PY32_BOR_CONFIG == 4
  borConfig = OB_BOR_ENABLE | OB_BOR_LEVEL_2p3_2p4;
#elif PY32_BOR_CONFIG == 5
  borConfig = OB_BOR_ENABLE | OB_BOR_LEVEL_2p5_2p6;
#elif PY32_BOR_CONFIG == 6
  borConfig = OB_BOR_ENABLE | OB_BOR_LEVEL_2p7_2p8;
#elif PY32_BOR_CONFIG == 7
  borConfig = OB_BOR_ENABLE | OB_BOR_LEVEL_2p9_3p0;
#elif PY32_BOR_CONFIG == 8
  borConfig = OB_BOR_ENABLE | OB_BOR_LEVEL_3p1_3p2;
#else
#error "Unsupported PY32_BOR_CONFIG value"
#endif

  const uint32_t borMask = FLASH_OPTR_BOR_EN | FLASH_OPTR_BOR_LEV;
  if ((FLASH->OPTR & borMask) == (borConfig & borMask)) {
    return;
  }

  FLASH_OBProgramInitTypeDef ob;
  ob.OptionType = OPTIONBYTE_USER;
  ob.USERType = OB_USER_BOR_EN | OB_USER_BOR_LEV;
  ob.USERConfig = borConfig;

  __disable_irq();
  (void)HAL_FLASH_Unlock();
  (void)HAL_FLASH_OB_Unlock();

  if (HAL_FLASH_OBProgram(&ob) == HAL_OK) {
    (void)HAL_FLASH_OB_Launch();
  }

  (void)HAL_FLASH_OB_Lock();
  (void)HAL_FLASH_Lock();
  __enable_irq();
}
#endif

WEAK void init(void)
{
  hw_config_init();
#if defined(PY32F0xx) && defined(FLASH_OPTR_BOR_EN) && defined(FLASH_OPTR_BOR_LEV) && (PY32_BOR_CONFIG >= 0) && PY32_APPLY_BOR_OPTION_BYTES
  py32_apply_bor_option_bytes();
#endif
}

#ifdef __cplusplus
}
#endif
