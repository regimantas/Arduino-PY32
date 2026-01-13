
#include <EasyInterval.h>

const int ledPin = 13;
EasyInterval blink(1000); // 1 second

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  if (blink.check()) {
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
}
