#pragma once

#include "Arduino.h"

#if defined(HAL_COMP_MODULE_ENABLED)

#include "py32f0xx_hal_comp.h"

// Internal minus input convenience tokens (Arduino-style)
// These map directly to device COMP input selections.
#ifndef VREF
#define VREF COMP_INPUT_MINUS_VREFINT
#endif
#ifndef VREF_1_4
#define VREF_1_4 COMP_INPUT_MINUS_1_4VREFINT
#endif
#ifndef VREF_1_2
#define VREF_1_2 COMP_INPUT_MINUS_1_2VREFINT
#endif
#ifndef VREF_3_4
#define VREF_3_4 COMP_INPUT_MINUS_3_4VREFINT
#endif
#ifndef VCC
#define VCC COMP_INPUT_MINUS_VCC
#endif
#ifndef TS
#define TS COMP_INPUT_MINUS_TS
#endif

// Convenience aliases to keep the callsite nice.
#ifndef Rising
#define Rising RISING
#endif
#ifndef Falling
#define Falling FALLING
#endif
#ifndef Change
#define Change CHANGE
#endif

// Hysteresis convenience values for Comparator::hysteresis()
#ifndef mV0
#define mV0 0
#endif
#ifndef mV20
#define mV20 20
#endif

// Digital filter convenience values for Comparator::filter()
#ifndef Samples0
#define Samples0 0
#endif
#ifndef Samples1
#define Samples1 1
#endif
#ifndef Samples2
#define Samples2 2
#endif
#ifndef Samples4
#define Samples4 4
#endif
#ifndef Samples8
#define Samples8 8
#endif
#ifndef Samples16
#define Samples16 16
#endif

// Power mode convenience values for Comparator::power()
#ifndef HighSpeed
#define HighSpeed COMP_POWERMODE_HIGHSPEED
#endif
#ifndef MediumSpeed
#define MediumSpeed COMP_POWERMODE_MEDIUMSPEED
#endif

class Comparator {
public:
  explicit Comparator(uint8_t instance);

  // plusPin: Arduino pin number (e.g. PA1)
  // minusPinOrInternal: Arduino pin number (e.g. PA0) OR one of {VREF, VREF_1_4, VREF_1_2, VREF_3_4, VCC, TS}
  // outPin: optional Arduino pin number used as a *software mirror* output (pass NC to disable)
  bool begin(uint32_t plusPin, uint32_t minusPinOrInternal, uint32_t outPin);

  // Convenience overload: omit the mirror pin.
  bool begin(uint32_t plusPin, uint32_t minusPinOrInternal) {
    return begin(plusPin, minusPinOrInternal, (uint32_t)NC);
  }

  // Enable/disable hysteresis. Currently maps to device on/off.
  // Pass mV20 to enable (mV0 to disable).
  void hysteresis(uint32_t hysteresis_mV);

  // Digital filter sample count. 0 disables. Clamped to 0..0xFFFF.
  void filter(uint16_t samples);

  // COMP_POWERMODE_HIGHSPEED / COMP_POWERMODE_MEDIUMSPEED
  void power(uint32_t mode);

  // mode: Rising/Falling/Change (or Arduino RISING/FALLING/CHANGE)
  void attachInterrupt(void (*callback)(), int mode);
  void detachInterrupt();

  // Invert comparator output polarity.
  // When enabled, comparator output is inverted (see device COMP output polarity).
  void invert(bool enable);

#if defined(HAL_TIM_MODULE_ENABLED) && !defined(HAL_TIM_MODULE_ONLY)
  // Start hardware PWM on a timer-backed pin and enable OCREF clear (OCREF_CLR_INT).
  // Note: OCREF clear availability depends on the timer and MCU routing.
  bool pwm(uint32_t outPin, uint32_t freqHz);

  // Enable/disable OCREF clear (PWM pulse cut) on a timer output.
  // This does NOT (re)configure PWM frequency/duty; it only toggles the timer's OCxREF clear enable.
  bool pwmClear(uint32_t outPin, bool enable = true);

  // Enable/disable a hardware PWM cutoff using the timer BREAK input.
  // On supported PY32 parts (e.g. PY32F002A, PY32F003) this can route COMP1/COMP2 into TIM1 BRK via SYSCFG.
  // When disabling, the implementation attempts to restore outputs (re-enables MOE).
  bool pwmBreak(uint32_t outPin, bool enable = true, bool activeHigh = true);

  // Set PWM duty in permille: 0..1000 (1000 = 100%).
  // Can be called before pwm(); value is latched and applied when PWM starts.
  void duty(uint16_t permille);
#endif

  // Returns true when comparator output is high.
  bool read();

  // Internal: shared IRQ entrypoint
  static void _handleIRQ();

private:
  bool applyConfig(bool restart);
  void updateOutPinFromHardware();

  static Comparator* s_comp1;
  static Comparator* s_comp2;

  uint8_t instance_ = 0;
  uint32_t outPin_ = (uint32_t)NC;

  void (*callback_)() = nullptr;

  // Cached configuration
  PinName plusPinName_ = NC;
  bool minusIsInternal_ = false;
  PinName minusPinName_ = NC;
  uint32_t minusInternalSel_ = COMP_INPUT_MINUS_VREFINT;

  uint32_t triggerMode_ = COMP_TRIGGERMODE_NONE;
  uint32_t hysteresisSel_ = COMP_HYSTERESIS_DISABLE;
  uint32_t powerMode_ = COMP_POWERMODE_HIGHSPEED;
  uint16_t filterCount_ = 0;

  bool outputInverted_ = false;

#if defined(HAL_TIM_MODULE_ENABLED) && !defined(HAL_TIM_MODULE_ONLY)
  PinName pwmPinName_ = NC;
  uint32_t pwmFreqHz_ = 0;
  uint16_t pwmDutyPermille_ = 500;
  uint8_t pwmChannel_ = 0; // Arduino channel [1..4]
  class HardwareTimer* pwmTimer_ = nullptr;
  bool ownsPwmTimer_ = false;
#endif

  bool started_ = false;
};

#endif // HAL_COMP_MODULE_ENABLED
