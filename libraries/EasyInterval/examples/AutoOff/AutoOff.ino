
#include <EasyInterval.h>

const int buttonPin = 2;
const int ledPin = 13;
EasyInterval autoOff(2000);

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    digitalWrite(ledPin, HIGH);
    autoOff.reset(); // restart 2-second timer
  }

  if (autoOff.expired()) {
    digitalWrite(ledPin, LOW);
  }
}
