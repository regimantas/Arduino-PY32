#pragma once

#include <Arduino.h>

extern "C" {
#include "py32f0xx_hal.h"
}

// Arduino-style minimal HAL I2C slave.
// Intended for PY32F0xx using I2C1 on PA2/PA3.
// Note: Do not use together with Wire (both define I2C1_IRQHandler).
class HalI2CSlave {
public:
  using OnReceiveFn = void (*)(uint8_t data);
  using OnRequestFn = uint8_t (*)();

  HalI2CSlave();

  void begin(uint8_t address7);
  void onReceive(OnReceiveFn fn);
  void onRequest(OnRequestFn fn);

  // internal (called from C callbacks / IRQ)
  I2C_HandleTypeDef* handle();
  void _rxCplt();
  void _txCplt();
  void _err();

  static HalI2CSlave* instance();

private:
  static HalI2CSlave* s_self;

  I2C_HandleTypeDef hi2c_{};
  uint8_t rxByte_ = 0;
  uint8_t txByte_ = 0;

  OnReceiveFn onReceive_ = nullptr;
  OnRequestFn onRequest_ = nullptr;

  void initPins_();
  void initI2C_(uint8_t address7);
  void startReceive_();
};
