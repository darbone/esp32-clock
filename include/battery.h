#pragma once
#include <Arduino.h>
#include "config.h"

// Voltage divider: 100k + 100k → bagi 2
// 18650 max 4.2V → ADC max 2.1V (masih aman buat ESP32 3.3V ADC)
// Kalau mau aman pakai 100k + 220k biar lebih kecil

#define BAT_MAX_V   4.20f
#define BAT_MIN_V   3.00f  // cutoff 18650
#define ADC_REF     3.30f
#define ADC_RES     4095.0f
#define DIVIDER     2.0f   // sesuai voltage divider lo

float batVoltage  = 0;
int   batPercent  = 0;

void batRead() {
  // Rata-rata 10 sample buat stabilin ADC ESP32 yang noisy
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(BAT_ADC_PIN);
    delay(2);
  }
  float raw    = sum / 10.0f;
  float adcV   = (raw / ADC_RES) * ADC_REF;
  batVoltage   = adcV * DIVIDER;

  // Clamp & convert ke persen
  batVoltage   = constrain(batVoltage, BAT_MIN_V, BAT_MAX_V);
  batPercent   = (int)(((batVoltage - BAT_MIN_V) / (BAT_MAX_V - BAT_MIN_V)) * 100.0f);
  batPercent   = constrain(batPercent, 0, 100);
}

// Warna baterai (RGB565)
uint16_t batColor() {
  if (batPercent > 50) return 0x07E0; // hijau
  if (batPercent > 20) return 0xFFE0; // kuning
  return 0xF800;                       // merah
}

bool batLow()      { return batPercent < 15; }
bool batCritical() { return batPercent < 5;  }
