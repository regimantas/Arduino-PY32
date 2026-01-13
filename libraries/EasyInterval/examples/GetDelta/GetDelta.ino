
#include <EasyInterval.h>

EasyInterval deltaTimer;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long dt = deltaTimer.get();
  Serial.print("Time since last check (ms): ");
  Serial.println(dt);
  delay(500);
}
