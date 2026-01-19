/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"
#include "PinConfigured.h"

#if (defined(PY32F002APRE) || defined(PY32F003PRE)) && defined(FLASH_OPTR_NRST_MODE)
#include "py32f0xx_hal_flash.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


extern uint32_t g_anOutputPinConfigured[MAX_NB_PORT];

bool isReset(void)
{
#if (defined(PY32F002APRE) || defined(PY32F003PRE)) && defined(FLASH_OPTR_NRST_MODE)
  // NRST_MODE==1 means PF2 is GPIO; NRST_MODE==0 means PF2 is reset.
  return ((FLASH->OPTR & FLASH_OPTR_NRST_MODE) == 0U);
#else
  // On unsupported MCUs/variants, treat as reset mode.
  return true;
#endif
}

#if (defined(PY32F002APRE) || defined(PY32F003PRE)) && defined(FLASH_OPTR_NRST_MODE)
static void py32_pf2_set_nrst_as_gpio(bool enableGpio)
{
  const bool isGpioNow = ((FLASH->OPTR & FLASH_OPTR_NRST_MODE) != 0U);
  if (enableGpio == isGpioNow) {
    return;
  }

  FLASH_OBProgramInitTypeDef ob;
  ob.OptionType = OPTIONBYTE_USER;
  ob.USERType = OB_USER_NRST_MODE;
  ob.USERConfig = enableGpio ? OB_RESET_MODE_GPIO : OB_RESET_MODE_RESET;

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

void pinMode(uint32_t ulPin, uint32_t ulMode)
{
  PinName p = digitalPinToPinName(ulPin);

  if (p != NC) {
#if (defined(PY32F002APRE) || defined(PY32F003PRE)) && defined(FLASH_OPTR_NRST_MODE)
    if (p == PF_2) {
      if (ulMode == NRST) {
        py32_pf2_set_nrst_as_gpio(false);
        return;
      }
      if (ulMode != INPUT_ANALOG) {
        py32_pf2_set_nrst_as_gpio(true);
      }
    }
#endif
    // If the pin that support PWM or DAC output, we need to turn it off
#if (defined(HAL_DAC_MODULE_ENABLED) && !defined(HAL_DAC_MODULE_ONLY)) ||\
    (defined(HAL_TIM_MODULE_ENABLED) && !defined(HAL_TIM_MODULE_ONLY))
    if (is_pin_configured(p, g_anOutputPinConfigured)) {
#if defined(HAL_DAC_MODULE_ENABLED) && !defined(HAL_DAC_MODULE_ONLY)
      if (pin_in_pinmap(p, PinMap_DAC)) {
        dac_stop(p);
      } else
#endif //HAL_DAC_MODULE_ENABLED && !HAL_DAC_MODULE_ONLY
#if defined(HAL_TIM_MODULE_ENABLED) && !defined(HAL_TIM_MODULE_ONLY)
        if (pin_in_pinmap(p, PinMap_TIM)) {
          pwm_stop(p);
        }
#endif //HAL_TIM_MODULE_ENABLED && !HAL_TIM_MODULE_ONLY
      {
        reset_pin_configured(p, g_anOutputPinConfigured);
      }
    }
#endif
    switch (ulMode) {
      case INPUT: /* INPUT_FLOATING */
        pin_function(p, PY32_PIN_DATA(PY32_MODE_INPUT, GPIO_NOPULL, 0));
        break;
      case INPUT_PULLUP:
        pin_function(p, PY32_PIN_DATA(PY32_MODE_INPUT, GPIO_PULLUP, 0));
        break;
      case INPUT_PULLDOWN:
        pin_function(p, PY32_PIN_DATA(PY32_MODE_INPUT, GPIO_PULLDOWN, 0));
        break;
      case INPUT_ANALOG:
        pin_function(p, PY32_PIN_DATA(PY32_MODE_ANALOG, GPIO_NOPULL, 0));
        break;
      case OUTPUT:
        pin_function(p, PY32_PIN_DATA(PY32_MODE_OUTPUT_PP, GPIO_NOPULL, 0));
        break;
      case OUTPUT_OPEN_DRAIN:
        pin_function(p, PY32_PIN_DATA(PY32_MODE_OUTPUT_OD, GPIO_NOPULL, 0));
        break;
      case NRST:
        Error_Handler();
        break;
      default:
        Error_Handler();
        break;
    }
  }
}

void digitalWrite(uint32_t ulPin, uint32_t ulVal)
{
  digitalWriteFast(digitalPinToPinName(ulPin), ulVal);
}

int digitalRead(uint32_t ulPin)
{
  return digitalReadFast(digitalPinToPinName(ulPin));
}

void digitalToggle(uint32_t ulPin)
{
  digitalToggleFast(digitalPinToPinName(ulPin));
}

#ifdef __cplusplus
}
#endif
