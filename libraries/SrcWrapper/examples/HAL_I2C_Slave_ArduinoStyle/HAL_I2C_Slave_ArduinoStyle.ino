#include <HalI2CSlave.h>

static volatile uint8_t gCounter = 0;
static volatile uint8_t gLed = 0;
static HalI2CSlave Slave;

static void onReceiveByte(uint8_t) {
  gCounter++;
  gLed ^= 1;
  digitalWrite(PA1, gLed ? HIGH : LOW);
}

static uint8_t onRequestByte() {
  return gCounter;
}

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
  //s->handleRxComplete_();
}

extern "C" void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  auto* s = HalI2CSlave::instance();
  if (!s) return;
  if (hi2c->Instance != I2C1) return;
  //s->handleTxComplete_();
}

extern "C" void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
  auto* s = HalI2CSlave::instance();
  if (!s) return;
  if (hi2c->Instance != I2C1) return;
  //s->handleError_();
}

void setup() {
  pinMode(PA1, OUTPUT);
  digitalWrite(PA1, LOW);
  Slave.onReceive(onReceiveByte);
  Slave.onRequest(onRequestByte);
  Slave.begin(0x12);
}

void loop() {
  // All I2C traffic handled by interrupts/callbacks.
}
