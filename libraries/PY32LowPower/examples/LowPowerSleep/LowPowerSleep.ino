#include <PY32LowPower.h>

PY32LowPower lowPower;

void setup() {
    lowPower.begin(); // Initialize low power settings
    pinMode(PA7, OUTPUT);
    digitalWrite(PA7, LOW); // Ensure LED is off initially
}

void loop() {
    digitalWrite(PA7, HIGH); // Turn on LED
    lowPower.deepSleep(1000); // Sleep for 1000 ms (1 second)
    digitalWrite(PA7, LOW);  // Turn off LED
    lowPower.deepSleep(1000); // Sleep for 1000 ms (1 second)
}