/**
  ******************************************************************************
  * @file    py32f0xx.h
  * @brief   CMSIS PY32F0xx Device Peripheral Access Layer Header File.
  * @version v1.0.0
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup py32f0xx
  * @{
  */

#ifndef __PY32F0XX_H
#define __PY32F0XX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @addtogroup Library_configuration_section
  * @{
  */

/**
  * @brief PY32 Family
  */
#if !defined  (PY32F0)
#define PY32F0
#endif /* PY32F0 */

#if (defined(PY32F030x3) || defined(PY32F030x4) || defined(PY32F030x6) || defined(PY32F030x7) || defined(PY32F030x8))
#define PY32F030PRE
#elif (defined(PY32F040x6) || defined(PY32F040x8) || defined(PY32F040x9) || defined(PY32F040xB))
#define PY32F040PRE
#elif (defined(PY32F040Cx6) || defined(PY32F040Cx8) || defined(PY32F040Cx9) || defined(PY32F040CxB))
#define PY32F040CPRE
#elif (defined(PY32F003x4) || defined(PY32F003x6) || defined(PY32F003x8))
#define PY32F003PRE
#elif (defined(PY32F072xB))
#define PY32F072PRE
#elif (defined(PY32F002Ax5))
#define PY32F002APRE
#endif

/* Uncomment the line below according to the target PY32 device used in your
   application
  */

#if !defined (PY32F030x3) && !defined (PY32F030x4) && !defined (PY32F030x6) && !defined (PY32F030x7) && !defined (PY32F030x8) && !defined (PY32F030xx) && \
    !defined (PY32F003x4) && !defined (PY32F003x6) && !defined (PY32F003x8) && \
    !defined (PY32F072xB) && \
  !defined (PY32F002Ax5) && \
  !defined (PY32F040x6) && !defined (PY32F040x8) && !defined (PY32F040x9) && !defined (PY32F040xB) && \
  !defined (PY32F040Cx6) && !defined (PY32F040Cx8) && !defined (PY32F040Cx9) && !defined (PY32F040CxB)
/* #define PY32F030x3  */  /*!< PY32F030x3  Devices (PY32F030xx  microcontrollers where the Flash memory is 8 Kbytes)               */
/* #define PY32F030x4  */  /*!< PY32F030x4  Devices (PY32F030xx  microcontrollers where the Flash memory is 16 Kbytes)              */
/* #define PY32F030x6  */  /*!< PY32F030x6  Devices (PY32F030xx  microcontrollers where the Flash memory is 32 Kbytes)              */
/* #define PY32F030x7  */  /*!< PY32F030x7  Devices (PY32F030xx  microcontrollers where the Flash memory is 48 Kbytes)              */
/* #define PY32F030x8  */  /*!< PY32F030x8  Devices (PY32F030xx  microcontrollers where the Flash memory is 64 Kbytes)              */
/* #define PY32F003x4  */  /*!< PY32F003x4  Devices (PY32F003xx  microcontrollers where the Flash memory is 16 Kbytes)              */
/* #define PY32F003x6  */  /*!< PY32F003x6  Devices (PY32F003xx  microcontrollers where the Flash memory is 32 Kbytes)              */
/* #define PY32F003x8  */  /*!< PY32F003x8  Devices (PY32F003xx  microcontrollers where the Flash memory is 64 Kbytes)              */
/* #define PY32F072xB  */  /*!< PY32F072xB  Devices (PY32F072xx  microcontrollers where the Flash memory is 128 Kbytes)             */
/* #define PY32F002Ax5 */  /*!< PY32F002Ax5 Devices (PY32F002Ax5 microcontrollers where the Flash memory is 20 Kbytes)              */
/* #define PY32F040x6 */   /*!< PY32F040x6  Devices (PY32F040xx  microcontrollers where the Flash memory is 32 Kbytes)              */
/* #define PY32F040x8 */   /*!< PY32F040x8  Devices (PY32F040xx  microcontrollers where the Flash memory is 64 Kbytes)              */
/* #define PY32F040x9 */   /*!< PY32F040x9  Devices (PY32F040xx  microcontrollers where the Flash memory is 96 Kbytes)              */
/* #define PY32F040xB */   /*!< PY32F040xB  Devices (PY32F040xx  microcontrollers where the Flash memory is 128 Kbytes)             */
/* #define PY32F040Cx6 */  /*!< PY32F040Cx6 Devices (PY32F040Cxx microcontrollers where the Flash memory is 32 Kbytes)              */
/* #define PY32F040Cx8 */  /*!< PY32F040Cx8 Devices (PY32F040Cxx microcontrollers where the Flash memory is 64 Kbytes)              */
/* #define PY32F040Cx9 */  /*!< PY32F040Cx9 Devices (PY32F040Cxx microcontrollers where the Flash memory is 96 Kbytes)              */
/* #define PY32F040CxB */  /*!< PY32F040CxB Devices (PY32F040Cxx microcontrollers where the Flash memory is 128 Kbytes)             */
#endif

/*  Tip: To avoid modifying this file each time you need to switch between these
        devices, you can define the device in your toolchain compiler preprocessor.
  */
#if !defined  (USE_HAL_DRIVER)
/**
 * @brief Comment the line below if you will not use the peripherals drivers.
   In this case, these drivers will not be included and the application code will
   be based on direct access to peripherals registers
   */
/*#define USE_HAL_DRIVER */
#endif /* USE_HAL_DRIVER */

/**
  * @brief CMSIS Device version number V1.0.0
  */
#define __PY32F0_DEVICE_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __PY32F0_DEVICE_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __PY32F0_DEVICE_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __PY32F0_DEVICE_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __PY32F0_DEVICE_VERSION        ((__PY32F0_DEVICE_VERSION_MAIN << 24)\
                                        |(__PY32F0_DEVICE_VERSION_SUB1 << 16)\
                                        |(__PY32F0_DEVICE_VERSION_SUB2 << 8 )\
                                        |(__PY32F0_DEVICE_VERSION_RC))

/**
  * @}
  */

/** @addtogroup Device_Included
  * @{
  */

#if defined(PY32F030x3)
#include "py32f030x3.h"
#elif defined(PY32F030x4)
#include "py32f030x4.h"
#elif defined(PY32F030x6)
#include "py32f030x6.h"
#elif defined(PY32F030x7)
#include "py32f030x7.h"
#elif defined(PY32F030x8)
#include "py32f030x8.h"
#elif defined(PY32F003x4)
#include "py32f003x4.h"
#elif defined(PY32F003x6)
#include "py32f003x6.h"
#elif defined(PY32F003x8)
#include "py32f003x8.h"
#elif defined(PY32F072xB)
#include "py32f072xB.h"
#elif defined(PY32F002Ax5)
#include "py32f002ax5.h"
#elif defined(PY32F040x6)
#include "py32f040x6.h"
#elif defined(PY32F040x8)
#include "py32f040x8.h"
#elif defined(PY32F040x9)
#include "py32f040x9.h"
#elif defined(PY32F040xB)
#include "py32f040xB.h"
#elif defined(PY32F040Cx6)
#include "py32f040cx6.h"
#elif defined(PY32F040Cx8)
#include "py32f040cx8.h"
#elif defined(PY32F040Cx9)
#include "py32f040cx9.h"
#elif defined(PY32F040CxB)
#include "py32f040cxB.h"
#else
#error "Please select first the target PY32F0xx device used in your application (in py32f0xx.h file)"
#endif

#if defined(COMP_CSR_INNSEL) && !defined(COMP_CSR_INMSEL)
#define COMP_CSR_INMSEL_Pos COMP_CSR_INNSEL_Pos
#define COMP_CSR_INMSEL_Msk COMP_CSR_INNSEL_Msk
#define COMP_CSR_INMSEL     COMP_CSR_INNSEL
#define COMP_CSR_INMSEL_0   COMP_CSR_INNSEL_0
#define COMP_CSR_INMSEL_1   COMP_CSR_INNSEL_1
#define COMP_CSR_INMSEL_2   COMP_CSR_INNSEL_2
#define COMP_CSR_INMSEL_3   COMP_CSR_INNSEL_3
#endif

#ifndef COMP_CSR_LOCK
#define COMP_CSR_LOCK       (0x00000000UL)
#endif

#ifndef DBGMCU_IDCODE_DEV_ID
#define DBGMCU_IDCODE_DEV_ID_Pos (0U)
#define DBGMCU_IDCODE_DEV_ID_Msk (0xFFFUL << DBGMCU_IDCODE_DEV_ID_Pos)
#define DBGMCU_IDCODE_DEV_ID     DBGMCU_IDCODE_DEV_ID_Msk
#endif

#if !defined(FLASH_OPTR_BOR_EN) && defined(FLASH_SDKR_BOR_EN)
#define FLASH_OPTR_BOR_EN        FLASH_SDKR_BOR_EN
#define FLASH_OPTR_BOR_LEV       FLASH_SDKR_BOR_LEV
#define FLASH_OPTR_BOR_LEV_0     FLASH_SDKR_BOR_LEV_0
#define FLASH_OPTR_BOR_LEV_1     FLASH_SDKR_BOR_LEV_1
#define FLASH_OPTR_BOR_LEV_2     FLASH_SDKR_BOR_LEV_2
#endif

#ifndef PWR_CR1_MRRDY_TIME
#define PWR_CR1_MRRDY_TIME       (0x00000000UL)
#endif
#ifndef PWR_CR1_HSION_CTRL
#define PWR_CR1_HSION_CTRL       (0x00000000UL)
#endif
#ifndef PWR_CR1_SRAM_RETV
#define PWR_CR1_SRAM_RETV        (0x00000000UL)
#endif

#ifndef RCC_ECSCR_HSE_FREQ_Msk
#define RCC_ECSCR_HSE_FREQ_Msk   (0x00000000UL)
#endif
#ifndef RCC_ECSCR_HSE_FREQ_0
#define RCC_ECSCR_HSE_FREQ_0     (0x00000000UL)
#endif
#ifndef RCC_ECSCR_HSE_FREQ_1
#define RCC_ECSCR_HSE_FREQ_1     (0x00000000UL)
#endif
#ifndef RCC_ECSCR_HSE_FREQ
#define RCC_ECSCR_HSE_FREQ       RCC_ECSCR_HSE_FREQ_Msk
#endif

#ifndef SYSCFG_CFGR2_ETR_SRC_TIM1
#define SYSCFG_CFGR2_ETR_SRC_TIM1 (0x00000000UL)
#endif
#ifndef SYSCFG_CFGR3_DMA1_ACKLVL
#define SYSCFG_CFGR3_DMA1_ACKLVL (0x00000000UL)
#endif
#ifndef SYSCFG_CFGR3_DMA2_ACKLVL
#define SYSCFG_CFGR3_DMA2_ACKLVL (0x00000000UL)
#endif
#ifndef SYSCFG_CFGR3_DMA3_ACKLVL
#define SYSCFG_CFGR3_DMA3_ACKLVL (0x00000000UL)
#endif

#ifndef SPI_CR2_FRXTH
#define SPI_CR2_FRXTH            (0x00000000UL)
#endif
#ifndef SPI_CR2_LDMA_RX
#define SPI_CR2_LDMA_RX          (0x00000000UL)
#endif
#ifndef SPI_CR2_LDMA_RX_Pos
#define SPI_CR2_LDMA_RX_Pos      (0U)
#endif
#ifndef SPI_CR2_LDMA_TX
#define SPI_CR2_LDMA_TX          (0x00000000UL)
#endif
#ifndef SPI_CR2_LDMA_TX_Pos
#define SPI_CR2_LDMA_TX_Pos      (0U)
#endif

#ifndef GPIO_AF0_SPI1
#define GPIO_AF0_SPI1            (0x0U)
#endif
#ifndef GPIO_AF1_TIM3
#define GPIO_AF1_TIM3            (0x1U)
#endif
#ifndef GPIO_AF1_USART1
#define GPIO_AF1_USART1          (0x1U)
#endif
#ifndef GPIO_AF2_TIM1
#define GPIO_AF2_TIM1            (0x2U)
#endif
#ifndef GPIO_AF2_TIM16
#define GPIO_AF2_TIM16           (0x2U)
#endif
#ifndef GPIO_AF4_TIM14
#define GPIO_AF4_TIM14           (0x4U)
#endif
#ifndef GPIO_AF4_USART2
#define GPIO_AF4_USART2          (0x4U)
#endif
#ifndef GPIO_AF6_I2C
#define GPIO_AF6_I2C             (0x6U)
#endif

#if !defined(I2C_BASE) && defined(I2C1_BASE)
#define I2C_BASE I2C1_BASE
#define I2C      I2C1
#endif
#if !defined(RCC_APBENR1_I2CEN) && defined(RCC_APBENR1_I2C1EN)
#define RCC_APBENR1_I2CEN RCC_APBENR1_I2C1EN
#endif
#if !defined(RCC_APBRSTR1_I2CRST) && defined(RCC_APBRSTR1_I2C1RST)
#define RCC_APBRSTR1_I2CRST RCC_APBRSTR1_I2C1RST
#endif

/**
  * @}
  */

/** @addtogroup Exported_types
  * @{
  */
typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
  ERROR = 0,
  SUCCESS = !ERROR
} ErrorStatus;

/**
  * @}
  */


/** @addtogroup Exported_macros
  * @{
  */
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define HW32_REG(ADDRESS)     ( * ((volatile unsigned           int * )(ADDRESS)))

#define HW16_REG(ADDRESS)     ( * ((volatile unsigned short     int * )(ADDRESS)))

#define HW8_REG(ADDRESS)      ( * ((volatile unsigned          char * )(ADDRESS)))

/**
  * @}
  */

#if defined (USE_HAL_DRIVER)
 #include "py32f0xx_hal.h"
#endif /* USE_HAL_DRIVER */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PY32F0xx_H */
/**
  * @}
  */

/**
  * @}
  */


/************************ (C) COPYRIGHT Puya *****END OF FILE******************/

