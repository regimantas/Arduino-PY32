
#include <EasyInterval.h>

EasyInterval myTimer(500); // like Metro
// Metro myTimer(500);     // same logic if Metro used

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (myTimer.check()) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}
