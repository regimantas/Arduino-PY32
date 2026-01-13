// EasyInterval - simple interval timer for Arduino projects
// Author: Regimantas Baublys

#ifndef EASYINTERVAL_H
#define EASYINTERVAL_H

#include <Arduino.h>  // Required for uint8_t and millis()

class EasyInterval {
private:
    unsigned long lastTime;
    unsigned long intervalMs;
    bool enabled = true;

    // Phased mode
    bool phasedMode = false;
    unsigned long phaseDurations[2] = {0, 0};
    uint8_t currentPhase = 0;

public:
    EasyInterval(unsigned long ms = 0);
    bool check();
    bool expired();
    bool checkCatchUp();
    bool running();
    void interval(unsigned long ms);
    void reset();
    unsigned long get();
    void setPhases(unsigned long ms1, unsigned long ms2);
    uint8_t phase();

    // Enable/disable methods
    void enable()   { enabled = true; }
    void disable()  { enabled = false; }
    bool isEnabled() const { return enabled; }
};

#endif
