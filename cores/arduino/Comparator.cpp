#include "Comparator.h"

#if defined(HAL_COMP_MODULE_ENABLED)

#include "pins_arduino.h"
#include "py32/PortNames.h"
#include "py32/PinNames.h"
#include "py32/PinNamesTypes.h"
#include "py32/pinmap.h"

#include "py32f0xx_hal_rcc.h"
#include "py32f0xx_ll_exti.h"

#if defined(HAL_TIM_MODULE_ENABLED) && !defined(HAL_TIM_MODULE_ONLY)
#include "HardwareTimer.h"
#include "py32f0xx_hal_tim.h"
#endif

Comparator* Comparator::s_comp1 = nullptr;
Comparator* Comparator::s_comp2 = nullptr;

static COMP_TypeDef* instanceToRegs(uint8_t instance)
{
  return (instance == 1) ? COMP1 : COMP2;
}

static uint32_t instanceToExtiLine(uint8_t instance)
{
  return COMP_GET_EXTI_LINE((instance == 1) ? COMP1 : COMP2);
}

static bool isVrefMinusSel(uint32_t minusSel)
{
  return (minusSel == COMP_INPUT_MINUS_1_4VREFINT) || (minusSel == COMP_INPUT_MINUS_1_2VREFINT) ||
         (minusSel == COMP_INPUT_MINUS_3_4VREFINT) || (minusSel == COMP_INPUT_MINUS_VREFINT);
}

static void busyWaitUs(uint32_t us)
{
  if (us == 0) return;

#if defined(SystemCoreClock)
  __IO uint32_t wait_loop_index = 0UL;
  // Same strategy as typical MCU delay loops: split scaling to keep within 32-bit arithmetic.
  wait_loop_index = ((us / 10UL) * (SystemCoreClock / (100000UL * 2UL)));
  while (wait_loop_index != 0UL) {
    wait_loop_index--;
  }
#else
  // Fallback: Arduino delay if SystemCoreClock isn't available.
  delayMicroseconds(us);
#endif
}

static uint32_t mapPowerModeToCSR(uint32_t powerMode)
{
  // API-level values come from core/device headers:
  // - HighSpeed = COMP_POWERMODE_HIGHSPEED (0)
  // - MediumSpeed = COMP_POWERMODE_MEDIUMSPEED (COMP_CSR_PWRMODE_0)
  //
  // On PY32F002Ax5, field values observed to work are:
  // - HighSpeed => PWRMODE=2 (COMP_CSR_PWRMODE_1)
  // - MediumSpeed => PWRMODE=1 (COMP_CSR_PWRMODE_0)
#if defined(PY32F002Ax5)
  if (powerMode == COMP_POWERMODE_HIGHSPEED) return COMP_CSR_PWRMODE_1;
  if (powerMode == COMP_POWERMODE_MEDIUMSPEED) return COMP_CSR_PWRMODE_0;
  return (powerMode & COMP_CSR_PWRMODE_Msk);
#else
  return powerMode;
#endif
}

static bool isInternalMinus(uint32_t v)
{
  switch (v) {
    case COMP_INPUT_MINUS_1_4VREFINT:
    case COMP_INPUT_MINUS_1_2VREFINT:
    case COMP_INPUT_MINUS_3_4VREFINT:
    case COMP_INPUT_MINUS_VREFINT:
    case COMP_INPUT_MINUS_VCC:
    case COMP_INPUT_MINUS_TS:
      return true;
    default:
      return false;
  }
}

static bool mapPlusInput(uint8_t instance, PinName plusPin, uint32_t* outSel)
{
  if (outSel == nullptr) return false;

  if (instance == 1) {
    if (plusPin == PB_8) { *outSel = COMP_INPUT_PLUS_IO1; return true; }
    if (plusPin == PB_2) { *outSel = COMP_INPUT_PLUS_IO2; return true; }
    if (plusPin == PA_1) { *outSel = COMP_INPUT_PLUS_IO3; return true; }
    return false;
  }

  if (instance == 2) {
    if (plusPin == PB_4) { *outSel = COMP_INPUT_PLUS_IO1; return true; }
    if (plusPin == PB_6) { *outSel = COMP_INPUT_PLUS_IO2; return true; }
    if (plusPin == PA_3) { *outSel = COMP_INPUT_PLUS_IO3; return true; }
#ifdef PF_3
    if (plusPin == PF_3) { *outSel = COMP_INPUT_PLUS_IO4; return true; }
#endif
    return false;
  }

  return false;
}

static bool mapMinusInputExternal(uint8_t instance, PinName minusPin, uint32_t* outSel)
{
  if (outSel == nullptr) return false;

  if (instance == 1) {
    if (minusPin == PB_1) { *outSel = COMP_INPUT_MINUS_IO1; return true; }
    if (minusPin == PA_0) { *outSel = COMP_INPUT_MINUS_IO3; return true; }
    return false;
  }

  if (instance == 2) {
    if (minusPin == PB_3) { *outSel = COMP_INPUT_MINUS_IO1; return true; }
    if (minusPin == PB_7) { *outSel = COMP_INPUT_MINUS_IO2; return true; }
    if (minusPin == PA_2) { *outSel = COMP_INPUT_MINUS_IO3; return true; }
    return false;
  }

  return false;
}

static void configurePinAnalog(PinName pn)
{
  if (pn == NC) return;
  GPIO_TypeDef* port = set_GPIO_Port_Clock(PY32_PORT(pn));
  if (port == nullptr) return;

  GPIO_InitTypeDef init = {};
  init.Pin = PY32_GPIO_PIN(pn);
  init.Mode = GPIO_MODE_ANALOG;
  init.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(port, &init);
}

static void configurePinOutput(PinName pn)
{
  if (pn == NC) return;
  GPIO_TypeDef* port = set_GPIO_Port_Clock(PY32_PORT(pn));
  if (port == nullptr) return;

  GPIO_InitTypeDef init = {};
  init.Pin = PY32_GPIO_PIN(pn);
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Pull = GPIO_NOPULL;
  init.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(port, &init);
}

static PinName arduinoPinToPinNameSafe(uint32_t pin)
{
  // Accept NC (PinName) or PNUM_NOT_DEFINED as "unused"
  if (pin == (uint32_t)NC) return NC;
  if (pin == (uint32_t)PNUM_NOT_DEFINED) return NC;

  // Prefer Arduino-style conversion first (handles both digital pins and PIN_Ax analog aliases).
  // If it doesn't resolve, then treat the value as a raw PinName.
  PinName pn = digitalPinToPinName(pin);
  if (pn != NC) return pn;

  return (PinName)pin;
}

Comparator::Comparator(uint8_t instance) : instance_(instance)
{
  if (instance_ == 1) s_comp1 = this;
  if (instance_ == 2) s_comp2 = this;
}

bool Comparator::begin(uint32_t plusPin, uint32_t minusPinOrInternal, uint32_t outPin)
{
  if (instance_ != 1 && instance_ != 2) return false;

  plusPinName_ = arduinoPinToPinNameSafe(plusPin);
  if (plusPinName_ == NC) return false;

  outPin_ = outPin;

  minusIsInternal_ = isInternalMinus(minusPinOrInternal);
  if (minusIsInternal_) {
    minusInternalSel_ = minusPinOrInternal;
    minusPinName_ = NC;
  } else {
    minusPinName_ = arduinoPinToPinNameSafe(minusPinOrInternal);
    if (minusPinName_ == NC) return false;
  }

  // Configure GPIO for comparator inputs (analog).
  configurePinAnalog(plusPinName_);
  if (!minusIsInternal_) configurePinAnalog(minusPinName_);

  // Optional software-mirrored output pin
  PinName outPn = arduinoPinToPinNameSafe(outPin_);
  if (outPn != NC) {
    configurePinOutput(outPn);
  }

  return applyConfig(true);
}

void Comparator::hysteresis(uint32_t hysteresis_mV)
{
  hysteresisSel_ = (hysteresis_mV >= 20) ? COMP_HYSTERESIS_ENABLE : COMP_HYSTERESIS_DISABLE;
  (void)applyConfig(started_);
}

void Comparator::filter(uint16_t samples)
{
  filterCount_ = samples;
  (void)applyConfig(started_);
}

void Comparator::power(uint32_t mode)
{
  powerMode_ = mode;
  (void)applyConfig(started_);
}

void Comparator::attachInterrupt(void (*callback)(), int mode)
{
  callback_ = callback;

  switch (mode) {
    case RISING:
      triggerMode_ = COMP_TRIGGERMODE_IT_RISING;
      break;
    case FALLING:
      triggerMode_ = COMP_TRIGGERMODE_IT_FALLING;
      break;
    case CHANGE:
      triggerMode_ = COMP_TRIGGERMODE_IT_RISING_FALLING;
      break;
    default:
      triggerMode_ = COMP_TRIGGERMODE_NONE;
      break;
  }

  // Enable shared ADC&COMP NVIC
#if defined(ADC_COMP_IRQn)
  HAL_NVIC_SetPriority(ADC_COMP_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(ADC_COMP_IRQn);
#endif

  (void)applyConfig(started_);
}

void Comparator::detachInterrupt()
{
  callback_ = nullptr;
  triggerMode_ = COMP_TRIGGERMODE_NONE;
  (void)applyConfig(started_);
}

void Comparator::invert(bool enable)
{
  outputInverted_ = enable;
  (void)applyConfig(started_);
}

#if defined(HAL_TIM_MODULE_ENABLED) && !defined(HAL_TIM_MODULE_ONLY)
static uint32_t dutyPermilleTo10bit(uint16_t permille)
{
  if (permille > 1000) permille = 1000;
  return (uint32_t)((((uint32_t)permille) * 1023u + 500u) / 1000u);
}

bool Comparator::pwm(uint32_t outPin, uint32_t freqHz)
{
  if (!started_) return false;
  if (freqHz == 0) return false;

  PinName outPn = arduinoPinToPinNameSafe(outPin);
  if (outPn == NC) return false;
  if (!pin_in_pinmap(outPn, PinMap_TIM)) return false;

  TIM_TypeDef* tim = (TIM_TypeDef*)pinmap_peripheral(outPn, PinMap_TIM);
  if (tim == nullptr) return false;

#if defined(TIM1)
  // Current implementation is intentionally conservative: TIM1 is expected.
  if (tim != TIM1) return false;
#endif

  uint32_t func = pinmap_function(outPn, PinMap_TIM);
  uint32_t channel = PY32_PIN_CHANNEL(func);
  if (channel < 1 || channel > 4) return false;

  // Ensure the timer supports OCref clear.
  if (!IS_TIM_OCXREF_CLEAR_INSTANCE(tim)) return false;

  uint32_t index = get_timer_index(tim);
  HardwareTimer* ht = nullptr;
  bool owns = false;

  if (index != UNKNOWN_TIMER && HardwareTimer_Handle[index] != NULL && HardwareTimer_Handle[index]->__this != NULL) {
    ht = (HardwareTimer*)HardwareTimer_Handle[index]->__this;
  } else {
    ht = new HardwareTimer(tim);
    owns = true;
  }

  // If we owned a previous timer, release it.
  if (pwmTimer_ && ownsPwmTimer_ && pwmTimer_ != ht) {
    delete pwmTimer_;
  }

  pwmTimer_ = ht;
  ownsPwmTimer_ = owns;
  pwmPinName_ = outPn;
  pwmFreqHz_ = freqHz;
  pwmChannel_ = (uint8_t)channel;

  // Configure PWM on the selected channel/pin.
  pwmTimer_->setMode(pwmChannel_, TIMER_OUTPUT_COMPARE_PWM1, pwmPinName_);
  pwmTimer_->setOverflow(pwmFreqHz_, HERTZ_FORMAT);
  pwmTimer_->setCaptureCompare(pwmChannel_, dutyPermilleTo10bit(pwmDutyPermille_), RESOLUTION_10B_COMPARE_FORMAT);
  pwmTimer_->resume();

  // Enable OCREF clear on this channel, using OCREF_CLR_INT.
  TIM_ClearInputConfigTypeDef clearCfg = {};
  clearCfg.ClearInputState = ENABLE;
  clearCfg.ClearInputSource = TIM_CLEARINPUTSOURCE_OCREFCLR;
  clearCfg.ClearInputPolarity = TIM_CLEARINPUTPOLARITY_NONINVERTED;
  clearCfg.ClearInputPrescaler = TIM_CLEARINPUTPRESCALER_DIV1;
  clearCfg.ClearInputFilter = 0;

  if (HAL_TIM_ConfigOCrefClear(pwmTimer_->getHandle(), &clearCfg, (uint32_t)pwmTimer_->getChannel(pwmChannel_)) != HAL_OK) {
    return false;
  }

  return true;
}

bool Comparator::pwmClear(uint32_t outPin, bool enable)
{
  if (!started_) return false;

  PinName outPn = arduinoPinToPinNameSafe(outPin);
  if (outPn == NC) return false;
  if (!pin_in_pinmap(outPn, PinMap_TIM)) return false;

  TIM_TypeDef* tim = (TIM_TypeDef*)pinmap_peripheral(outPn, PinMap_TIM);
  if (tim == nullptr) return false;

#if defined(TIM1)
  // Keep the same conservative timer expectation as pwm().
  if (tim != TIM1) return false;
#endif

  // Ensure the timer supports OCref clear.
  if (!IS_TIM_OCXREF_CLEAR_INSTANCE(tim)) return false;

  uint32_t func = pinmap_function(outPn, PinMap_TIM);
  uint32_t channel = PY32_PIN_CHANNEL(func);
  if (channel < 1 || channel > 4) return false;

  // Route OCREF clear to the internal OCREF_CLR input (not ETRF).
  // This matches the configuration used by pwm().
  if (enable) {
    CLEAR_BIT(tim->SMCR, TIM_SMCR_OCCS);
  }

  // Toggle per-channel OCxREF clear enable.
  switch (channel) {
    case 1:
      if (enable) {
        SET_BIT(tim->CCMR1, TIM_CCMR1_OC1CE);
      } else {
        CLEAR_BIT(tim->CCMR1, TIM_CCMR1_OC1CE);
      }
      break;
    case 2:
      if (enable) {
        SET_BIT(tim->CCMR1, TIM_CCMR1_OC2CE);
      } else {
        CLEAR_BIT(tim->CCMR1, TIM_CCMR1_OC2CE);
      }
      break;
    case 3:
      if (enable) {
        SET_BIT(tim->CCMR2, TIM_CCMR2_OC3CE);
      } else {
        CLEAR_BIT(tim->CCMR2, TIM_CCMR2_OC3CE);
      }
      break;
    case 4:
      if (enable) {
        SET_BIT(tim->CCMR2, TIM_CCMR2_OC4CE);
      } else {
        CLEAR_BIT(tim->CCMR2, TIM_CCMR2_OC4CE);
      }
      break;
    default:
      return false;
  }

  return true;
}

bool Comparator::pwmBreak(uint32_t outPin, bool enable, bool activeHigh)
{
  if (!started_) return false;
  if (instance_ != 1 && instance_ != 2) return false;

  PinName outPn = arduinoPinToPinNameSafe(outPin);
  if (outPn == NC) return false;
  if (!pin_in_pinmap(outPn, PinMap_TIM)) return false;

  TIM_TypeDef* tim = (TIM_TypeDef*)pinmap_peripheral(outPn, PinMap_TIM);
  if (tim == nullptr) return false;

#if defined(TIM1)
  // Keep the same conservative timer expectation as pwm()/pwmClear().
  if (tim != TIM1) return false;
#endif

  // This feature relies on SYSCFG routing bits that exist on some PY32 parts.
#if defined(SYSCFG) && (defined(SYSCFG_CFGR2_COMP1_BRK_TIM1) || defined(SYSCFG_CFGR2_COMP2_BRK_TIM1))
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  uint32_t routeBit = 0;
  if (instance_ == 1) {
#if defined(SYSCFG_CFGR2_COMP1_BRK_TIM1)
    routeBit = SYSCFG_CFGR2_COMP1_BRK_TIM1;
#else
    return false;
#endif
  } else {
#if defined(SYSCFG_CFGR2_COMP2_BRK_TIM1)
    routeBit = SYSCFG_CFGR2_COMP2_BRK_TIM1;
#else
    return false;
#endif
  }

  if (enable) {
    SET_BIT(SYSCFG->CFGR2, routeBit);
  } else {
    CLEAR_BIT(SYSCFG->CFGR2, routeBit);
  }

  if (enable) {
    // Enable break input and set desired polarity.
    SET_BIT(tim->BDTR, TIM_BDTR_BKE);
    if (activeHigh) {
      SET_BIT(tim->BDTR, TIM_BDTR_BKP);
    } else {
      CLEAR_BIT(tim->BDTR, TIM_BDTR_BKP);
    }
    // Automatic output enable: when break becomes inactive, MOE can be restored automatically
    // at the next update event. This makes "auto clear" behavior possible.
    SET_BIT(tim->BDTR, TIM_BDTR_AOE);

    // Clear any pending break flag (best-effort) and ensure main outputs are enabled.
    tim->SR = ~(TIM_SR_BIF);
    SET_BIT(tim->BDTR, TIM_BDTR_MOE);
  } else {
    // Disable break input and attempt to restore outputs.
    CLEAR_BIT(tim->BDTR, TIM_BDTR_BKE);

    // Disable automatic output enable when feature is disabled.
    CLEAR_BIT(tim->BDTR, TIM_BDTR_AOE);

    // Clear any pending break flag and re-enable main outputs.
    tim->SR = ~(TIM_SR_BIF);

    // Some parts behave more reliably if MOE is toggled.
    CLEAR_BIT(tim->BDTR, TIM_BDTR_MOE);
    SET_BIT(tim->BDTR, TIM_BDTR_MOE);
  }

  return true;
#else
  (void)outPin;
  (void)enable;
  (void)activeHigh;
  return false;
#endif
}

void Comparator::duty(uint16_t permille)
{
  if (permille > 1000) permille = 1000;
  pwmDutyPermille_ = permille;
  if (!pwmTimer_ || pwmChannel_ < 1 || pwmChannel_ > 4) return;

  pwmTimer_->setCaptureCompare(pwmChannel_, dutyPermilleTo10bit(pwmDutyPermille_), RESOLUTION_10B_COMPARE_FORMAT);
}

uint32_t Comparator::pwmTop() const
{
  if (!pwmTimer_) return 0;
  TIM_HandleTypeDef* htim = pwmTimer_->getHandle();
  if (htim == nullptr || htim->Instance == nullptr) return 0;
  return (uint32_t)(htim->Instance->ARR);
}

void Comparator::dutyRaw(uint32_t compareCounts)
{
  if (!pwmTimer_ || pwmChannel_ < 1 || pwmChannel_ > 4) return;

  TIM_HandleTypeDef* htim = pwmTimer_->getHandle();
  if (htim == nullptr || htim->Instance == nullptr) return;

  TIM_TypeDef* tim = htim->Instance;
  uint32_t top = (uint32_t)tim->ARR;
  if (compareCounts > top) compareCounts = top;

  switch (pwmChannel_) {
    case 1:
      tim->CCR1 = (uint32_t)compareCounts;
      break;
    case 2:
      tim->CCR2 = (uint32_t)compareCounts;
      break;
    case 3:
      tim->CCR3 = (uint32_t)compareCounts;
      break;
    case 4:
      tim->CCR4 = (uint32_t)compareCounts;
      break;
    default:
      break;
  }
}
#endif

bool Comparator::read()
{
  if (!started_) return false;

  COMP_TypeDef* regs = instanceToRegs(instance_);
  bool high = (READ_BIT(regs->CSR, COMP_CSR_COMP_OUT) != 0U);
  updateOutPinFromHardware();
  return high;
}

bool Comparator::applyConfig(bool restart)
{
  uint32_t inputPlusSel = 0;
  if (!mapPlusInput(instance_, plusPinName_, &inputPlusSel)) {
    return false;
  }

  uint32_t inputMinusSel = 0;
  if (minusIsInternal_) {
    inputMinusSel = minusInternalSel_;
  } else {
    if (!mapMinusInputExternal(instance_, minusPinName_, &inputMinusSel)) {
      return false;
    }
  }

  // Enable peripheral clock
#if defined(RCC_APBENR2_COMP1EN) && defined(RCC_APBENR2_COMP2EN)
  if (instance_ == 1) {
    SET_BIT(RCC->APBENR2, RCC_APBENR2_COMP1EN);
  } else {
    SET_BIT(RCC->APBENR2, RCC_APBENR2_COMP2EN);
  }
#endif

  COMP_TypeDef* regs = instanceToRegs(instance_);

  // Disable comparator while configuring (reliable on PY32F002A).
  // This matches the sequence used in known-good register-only implementations.
  (void)restart;
  CLEAR_BIT(regs->CSR, COMP_CSR_EN);
  started_ = false;

  // Configure digital filter.
  if (filterCount_ == 0) {
    CLEAR_BIT(regs->FR, COMP_FR_FLTEN);
  } else {
    uint32_t cnt = (filterCount_ > 0xFFFFu) ? 0xFFFFu : (uint32_t)filterCount_;
    WRITE_REG(regs->FR, (COMP_FR_FLTEN | (cnt << COMP_FR_FLTCNT_Pos)));
  }

  // Configure EXTI triggering for comparator output.
  uint32_t exti_line = instanceToExtiLine(instance_);
  if ((triggerMode_ & (COMP_EXTI_IT | COMP_EXTI_EVENT)) != 0UL) {
    if ((triggerMode_ & COMP_EXTI_RISING) != 0UL) {
      LL_EXTI_EnableRisingTrig(exti_line);
    } else {
      LL_EXTI_DisableRisingTrig(exti_line);
    }

    if ((triggerMode_ & COMP_EXTI_FALLING) != 0UL) {
      LL_EXTI_EnableFallingTrig(exti_line);
    } else {
      LL_EXTI_DisableFallingTrig(exti_line);
    }

    LL_EXTI_ClearFlag(exti_line);

    if ((triggerMode_ & COMP_EXTI_EVENT) != 0UL) {
      LL_EXTI_EnableEvent(exti_line);
    } else {
      LL_EXTI_DisableEvent(exti_line);
    }

    if ((triggerMode_ & COMP_EXTI_IT) != 0UL) {
      LL_EXTI_EnableIT(exti_line);
    } else {
      LL_EXTI_DisableIT(exti_line);
    }
  } else {
    LL_EXTI_DisableRisingTrig(exti_line);
    LL_EXTI_DisableFallingTrig(exti_line);
    LL_EXTI_DisableEvent(exti_line);
    LL_EXTI_DisableIT(exti_line);
    LL_EXTI_ClearFlag(exti_line);
  }

  // Determine whether the common Vrefint scaler bridge must be enabled.
  bool thisNeedsScaler = isVrefMinusSel(inputMinusSel);
  bool otherNeedsScaler = false;
  if (instance_ == 1) {
    if (Comparator::s_comp2 && Comparator::s_comp2->started_) {
      uint32_t otherMinusSel = Comparator::s_comp2->minusIsInternal_ ? Comparator::s_comp2->minusInternalSel_ : 0;
      // External minus never needs Vrefint scaler.
      otherNeedsScaler = Comparator::s_comp2->minusIsInternal_ && isVrefMinusSel(otherMinusSel);
    }
  } else {
    if (Comparator::s_comp1 && Comparator::s_comp1->started_) {
      uint32_t otherMinusSel = Comparator::s_comp1->minusIsInternal_ ? Comparator::s_comp1->minusInternalSel_ : 0;
      otherNeedsScaler = Comparator::s_comp1->minusIsInternal_ && isVrefMinusSel(otherMinusSel);
    }
  }
  bool scalerShouldBeOn = thisNeedsScaler || otherNeedsScaler;

#if defined(COMP12_COMMON)
  bool scalerWasOn = (READ_BIT(COMP12_COMMON->CSR_ODD, COMP_CSR_SCALER_EN) != 0U);
  if (scalerShouldBeOn) {
    SET_BIT(COMP12_COMMON->CSR_ODD, COMP_CSR_SCALER_EN);
  } else {
    CLEAR_BIT(COMP12_COMMON->CSR_ODD, COMP_CSR_SCALER_EN);
  }

  // Hysteresis is controlled via COMP1 CSR (common for COMP1/COMP2 on PY32F002A).
  MODIFY_REG(COMP12_COMMON->CSR_ODD, COMP_CSR_HYST, hysteresisSel_);

  // Disable window mode (this core API does not expose it).
  CLEAR_BIT(COMP12_COMMON->CSR_ODD, COMP_CSR_WINMODE);
  CLEAR_BIT(COMP12_COMMON->CSR_EVEN, COMP_CSR_WINMODE);
#else
  bool scalerWasOn = false;
#endif

  // If enabling scaler bridge for the first time, allow it to stabilize.
  if (scalerShouldBeOn && !scalerWasOn) {
    busyWaitUs(200);
  }

  // Configure instance CSR bits (do not touch lock/output read-only bits).
  uint32_t tmp_csr = (inputMinusSel | inputPlusSel | mapPowerModeToCSR(powerMode_) |
                      (outputInverted_ ? COMP_OUTPUTPOL_INVERTED : COMP_OUTPUTPOL_NONINVERTED));

  MODIFY_REG(regs->CSR,
             COMP_CSR_LOCK | COMP_CSR_COMP_OUT | COMP_CSR_PWRMODE | COMP_CSR_POLARITY | COMP_CSR_INPSEL | COMP_CSR_INMSEL | COMP_CSR_EN,
             tmp_csr);

  // Enable comparator and wait for startup.
  SET_BIT(regs->CSR, COMP_CSR_EN);
  busyWaitUs(80);

  started_ = true;
  updateOutPinFromHardware();
  return true;
}

void Comparator::updateOutPinFromHardware()
{
  PinName outPn = arduinoPinToPinNameSafe(outPin_);
  if (outPn == NC) return;

  COMP_TypeDef* regs = instanceToRegs(instance_);
  uint32_t level = (READ_BIT(regs->CSR, COMP_CSR_COMP_OUT) != 0U) ? COMP_OUTPUT_LEVEL_HIGH : COMP_OUTPUT_LEVEL_LOW;
  GPIO_TypeDef* port = get_GPIO_Port(PY32_PORT(outPn));
  if (port == nullptr) return;

  if (level == COMP_OUTPUT_LEVEL_HIGH) {
    HAL_GPIO_WritePin(port, PY32_GPIO_PIN(outPn), GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(port, PY32_GPIO_PIN(outPn), GPIO_PIN_RESET);
  }
}

void Comparator::_handleIRQ()
{
  if (s_comp1 && s_comp1->started_) {
    uint32_t line = instanceToExtiLine(1);
    if (LL_EXTI_IsActiveFlag(line)) {
      LL_EXTI_ClearFlag(line);
      s_comp1->updateOutPinFromHardware();
      if (s_comp1->callback_) s_comp1->callback_();
    }
  }
  if (s_comp2 && s_comp2->started_) {
    uint32_t line = instanceToExtiLine(2);
    if (LL_EXTI_IsActiveFlag(line)) {
      LL_EXTI_ClearFlag(line);
      s_comp2->updateOutPinFromHardware();
      if (s_comp2->callback_) s_comp2->callback_();
    }
  }
}

#endif // HAL_COMP_MODULE_ENABLED
