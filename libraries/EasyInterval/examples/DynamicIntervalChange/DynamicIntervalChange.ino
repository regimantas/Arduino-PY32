
#include <EasyInterval.h>

EasyInterval blinker(1000); // initial interval
bool fast = false;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (blinker.check()) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    fast = !fast;
    blinker.interval(fast ? 200 : 1000); // toggle interval
  }
}
