
#include <EasyInterval.h>

const int ledPin = 13;
EasyInterval timer;

void setup() {
  pinMode(ledPin, OUTPUT);
  timer.setPhases(1000, 3000); // ON 1s, OFF 3s
}

void loop() {
  if (timer.check()) {
    digitalWrite(ledPin, timer.phase() == 0 ? HIGH : LOW);
  }
}
