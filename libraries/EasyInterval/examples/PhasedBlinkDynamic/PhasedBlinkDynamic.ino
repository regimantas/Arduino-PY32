
#include <EasyInterval.h>

EasyInterval timer;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  timer.setPhases(1000, 3000);
}

void loop() {
  if (timer.check()) {
    digitalWrite(LED_BUILTIN, timer.phase() == 0 ? HIGH : LOW);
  }

  // simulate dynamic update
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'f') {
      timer.setPhases(200, 200);
    } else if (c == 's') {
      timer.setPhases(1000, 3000);
    }
  }
}
