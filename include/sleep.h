#pragma once
#include <Arduino.h>
#include "config.h"

unsigned long lastActivity  = 0;
bool          isSleeping    = false;
int           savedBrightness = 200;

extern int brightness;  // dari main

void resetSleepTimer() {
  lastActivity = millis();
  isSleeping   = false;
}

void wakeUp(int blChannel, int bright) {
  isSleeping = false;
  ledcWrite(blChannel, bright);
  lastActivity = millis();
}

// Panggil di loop() setiap saat
// Return true = baru masuk sleep (buat clear screen di main)
bool checkAutoSleep(int blChannel, int bright) {
  unsigned long elapsed = millis() - lastActivity;
  unsigned long timeoutMs = (unsigned long)SLEEP_TIMEOUT_S * 1000;
  unsigned long dimMs     = timeoutMs - (unsigned long)DIM_BEFORE_S * 1000;

  if (isSleeping) return false;

  // Mulai dim sebelum sleep
  if (elapsed > dimMs && elapsed < timeoutMs) {
    // Dim proporsional makin gelap
    int dimVal = map(elapsed, dimMs, timeoutMs, bright, 5);
    ledcWrite(blChannel, dimVal);
    return false;
  }

  // Masuk sleep
  if (elapsed >= timeoutMs) {
    isSleeping = true;
    ledcWrite(blChannel, 0);  // backlight off
    return true;              // sinyal ke main buat clear screen
  }

  return false;
}
