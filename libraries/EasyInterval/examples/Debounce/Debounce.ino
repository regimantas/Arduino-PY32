
#include <EasyInterval.h>

const int buttonPin = 2;
const int ledPin = 13;
EasyInterval debounce;
int lastState = HIGH;
bool ledState = false;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  int state = digitalRead(buttonPin);
  if (state != lastState && debounce.get() > 50) {
    lastState = state;
    if (state == LOW) {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
    }
  }
}
