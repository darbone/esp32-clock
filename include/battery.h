#pragma once
#include <Arduino.h>
#include "config.h"

float batVoltage = 0;
int   batPercent = 0;

void batRead() {
  long sum = 0;
  for (int i = 0; i < 10; i++) { sum += analogRead(BAT_ADC_PIN); delay(2); }
  float adcV   = (sum / 10.0f / 4095.0f) * 3.3f;
  batVoltage   = adcV * 2.0f;
  batVoltage   = constrain(batVoltage, 3.0f, 4.2f);
  batPercent   = (int)(((batVoltage - 3.0f) / 1.2f) * 100.0f);
  batPercent   = constrain(batPercent, 0, 100);
}
