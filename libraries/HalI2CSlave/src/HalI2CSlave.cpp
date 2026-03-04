#include "HalI2CSlave.h"

HalI2CSlave* HalI2CSlave::s_self = nullptr;

HalI2CSlave::HalI2CSlave() {
  // singleton-style: last constructed wins
  s_self = this;
}

HalI2CSlave* HalI2CSlave::instance() { return s_self; }

I2C_HandleTypeDef* HalI2CSlave::handle() { return &hi2c_; }

void HalI2CSlave::onReceive(OnReceiveFn fn) { onReceive_ = fn; }
void HalI2CSlave::onRequest(OnRequestFn fn) { onRequest_ = fn; }

void HalI2CSlave::begin(uint8_t address7) {
  s_self = this;
  initPins_();
  initI2C_(address7);
  startReceive_();
}

void HalI2CSlave::initPins_() {
  // GPIOA clock
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

#if defined(GPIO_AF12_I2C)
  const uint32_t i2cAf = GPIO_AF12_I2C;
#elif defined(GPIO_AF6_I2C)
  const uint32_t i2cAf = GPIO_AF6_I2C;
#else
#  error "No GPIO AF define for I2C found"
#endif

  GPIO_InitTypeDef i2c{};
  i2c.Pin = GPIO_PIN_2 | GPIO_PIN_3; // PA2=SDA, PA3=SCL
  i2c.Mode = GPIO_MODE_AF_OD;
  i2c.Pull = GPIO_PULLUP;            // still requires external pull-ups
  i2c.Speed = GPIO_SPEED_FREQ_HIGH;
  i2c.Alternate = i2cAf;
  HAL_GPIO_Init(GPIOA, &i2c);
}

void HalI2CSlave::initI2C_(uint8_t address7) {
#if !defined(I2C1)
#  error "HalI2CSlave expects I2C1 (PY32F0xx)."
#endif

  RCC->APBENR1 |= RCC_APBENR1_I2CEN;
  RCC->APBRSTR1 |= RCC_APBRSTR1_I2CRST;
  RCC->APBRSTR1 &= ~RCC_APBRSTR1_I2CRST;

  hi2c_.Instance = I2C1;
  hi2c_.Init.ClockSpeed = 100000;
  hi2c_.Init.DutyCycle = I2C_DUTYCYCLE_2;

  // PY32: OAR1 ADD1_7 mask is 0xFE, so use 7-bit address << 1.
  hi2c_.Init.OwnAddress1 = (uint32_t)(address7 << 1);

  hi2c_.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c_.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(&hi2c_) != HAL_OK) {
    Error_Handler();
  }

#if defined(I2C1_IRQn)
  HAL_NVIC_SetPriority(I2C1_IRQn, 1, 0);
  HAL_NVIC_ClearPendingIRQ(I2C1_IRQn);
  HAL_NVIC_EnableIRQ(I2C1_IRQn);
#else
#  error "I2C1_IRQn is not defined for this target"
#endif
}

void HalI2CSlave::startReceive_() {
  (void)HAL_I2C_Slave_Receive_IT(&hi2c_, &rxByte_, 1);
}

void HalI2CSlave::_rxCplt() {
  if (onReceive_) onReceive_(rxByte_);
  if (onRequest_) txByte_ = onRequest_();
  (void)HAL_I2C_Slave_Transmit_IT(&hi2c_, &txByte_, 1);
}

void HalI2CSlave::_txCplt() { startReceive_(); }
void HalI2CSlave::_err() { startReceive_(); }

extern "C" void I2C1_IRQHandler(void) {
  auto* s = HalI2CSlave::instance();
  if (!s) return;
  I2C_HandleTypeDef* h = s->handle();
  HAL_I2C_EV_IRQHandler(h);
  HAL_I2C_ER_IRQHandler(h);
}

extern "C" void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  auto* s = HalI2CSlave::instance();
  if (!s) return;
  if (hi2c->Instance != I2C1) return;
  s->_rxCplt();
}

extern "C" void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  auto* s = HalI2CSlave::instance();
  if (!s) return;
  if (hi2c->Instance != I2C1) return;
  s->_txCplt();
}

extern "C" void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
  auto* s = HalI2CSlave::instance();
  if (!s) return;
  if (hi2c->Instance != I2C1) return;
  s->_err();
}
