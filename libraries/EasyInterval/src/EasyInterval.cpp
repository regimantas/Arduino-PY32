// EasyInterval - simple interval timer for Arduino projects
// Author: Regimantas Baublys

#include "EasyInterval.h"

EasyInterval::EasyInterval(unsigned long ms) {
  lastTime = millis();
  intervalMs = ms;
}

bool EasyInterval::check() {
  if (!enabled) return false;
  unsigned long now = millis();
  if (now - lastTime >= intervalMs) {
    lastTime = now;
    if (phasedMode) {
      currentPhase = 1 - currentPhase;
      intervalMs = phaseDurations[currentPhase];
    }
    return true;
  }
  return false;
}

bool EasyInterval::expired() {
  if (!enabled) return false;
  return (millis() - lastTime >= intervalMs);
}

bool EasyInterval::checkCatchUp() {
  if (!enabled) return false;
  unsigned long now = millis();
  if (now - lastTime >= intervalMs) {
    lastTime += intervalMs;
    if (phasedMode) {
      currentPhase = 1 - currentPhase;
      intervalMs = phaseDurations[currentPhase];
    }
    return true;
  }
  return false;
}

bool EasyInterval::running() {
  if (!enabled) return false;
  return (millis() - lastTime < intervalMs);
}

void EasyInterval::interval(unsigned long ms) {
  intervalMs = ms;
  phasedMode = false;
}

void EasyInterval::reset() {
  lastTime = millis();
}

unsigned long EasyInterval::get() {
  if (!enabled) return 0;
  unsigned long now = millis();
  unsigned long elapsed = now - lastTime;
  lastTime = now;
  return elapsed;
}

// Set two phases (e.g., 1s ON, 3s OFF)
void EasyInterval::setPhases(unsigned long ms1, unsigned long ms2) {
  phaseDurations[0] = ms1;
  phaseDurations[1] = ms2;
  currentPhase = 0;
  intervalMs = ms1;
  phasedMode = true;
  lastTime = millis();
}

// Returns the current phase (0 or 1)
uint8_t EasyInterval::phase() {
  return currentPhase;
}
