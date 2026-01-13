
#include <EasyInterval.h>

EasyInterval heartbeat(1000);
bool enabled = true;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (heartbeat.check()) {
    Serial.println("Heartbeat...");
  }

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'e') {
      heartbeat.enable();
      Serial.println("Enabled timer");
    } else if (c == 'd') {
      heartbeat.disable();
      Serial.println("Disabled timer");
    }
  }
}
