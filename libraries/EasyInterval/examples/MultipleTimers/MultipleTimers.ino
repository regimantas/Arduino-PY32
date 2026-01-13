
#include <EasyInterval.h>

EasyInterval timer1(1000); // 1s
EasyInterval timer2(3000); // 3s

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (timer1.check()) {
    Serial.println("Timer1 tick");
  }

  if (timer2.check()) {
    Serial.println("Timer2 tick");
  }
}
