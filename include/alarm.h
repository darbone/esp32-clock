#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

// Rename struct & variable → alarmCfg biar ga bentrok sama alarm() bawaan Arduino
struct AlarmConfig {
  int  hour    = ALARM_DEFAULT_H;
  int  minute  = ALARM_DEFAULT_M;
  bool enabled = true;
  bool ringing = false;
  bool snoozed = false;
  unsigned long snoozeUntil = 0;
};

AlarmConfig alarmCfg;
Preferences prefs;

// ===== Simpan/Load dari NVS =====
void alarmSave() {
  prefs.begin("alarm", false);
  prefs.putInt("h",   alarmCfg.hour);
  prefs.putInt("m",   alarmCfg.minute);
  prefs.putBool("en", alarmCfg.enabled);
  prefs.end();
}

void alarmLoad() {
  prefs.begin("alarm", true);
  alarmCfg.hour    = prefs.getInt("h",   ALARM_DEFAULT_H);
  alarmCfg.minute  = prefs.getInt("m",   ALARM_DEFAULT_M);
  alarmCfg.enabled = prefs.getBool("en", true);
  prefs.end();
}

// ===== Buzzer patterns =====
void buzzTone(int pin, int freq, int dur) {
  ledcSetup(1, freq, 8);
  ledcAttachPin(pin, 1);
  ledcWrite(1, 128);
  delay(dur);
  ledcWrite(1, 0);
}

void playBeep(int pin) {
  int seq[][2] = {{2000,150},{0,80},{2000,150},{0,80},{2500,350},{0,400}};
  for (auto& s : seq) {
    if (s[0] > 0) buzzTone(pin, s[0], s[1]);
    else delay(s[1]);
  }
}

void playMelody(int pin) {
  int notes[][2] = {
    {523,150},{659,150},{784,150},{1047,300},
    {784,150},{523,150},{659,300},{0,200}
  };
  for (auto& n : notes) {
    if (n[0] > 0) buzzTone(pin, n[0], n[1]);
    else delay(n[1]);
  }
}

void playSOS(int pin) {
  for (int i = 0; i < 3; i++) { buzzTone(pin,1000,100); delay(100); }
  delay(200);
  for (int i = 0; i < 3; i++) { buzzTone(pin,1000,300); delay(100); }
  delay(200);
  for (int i = 0; i < 3; i++) { buzzTone(pin,1000,100); delay(100); }
}

// ===== Cek alarm setiap loop =====
void checkAlarm(int pin, int curHour, int curMin, int buzzerMode = 0) {
  if (!alarmCfg.enabled) return;

  if (alarmCfg.snoozed && millis() > alarmCfg.snoozeUntil) {
    alarmCfg.snoozed = false;
  }
  if (alarmCfg.snoozed) return;

  if (curHour == alarmCfg.hour && curMin == alarmCfg.minute) {
    alarmCfg.ringing = true;
    switch (buzzerMode) {
      case 0: playBeep(pin);   break;
      case 1: playMelody(pin); break;
      case 2: playSOS(pin);    break;
    }
  }
}

void snoozeAlarm() {
  alarmCfg.ringing   = false;
  alarmCfg.snoozed   = true;
  alarmCfg.snoozeUntil = millis() + 5UL * 60000UL;
}

void dismissAlarm() {
  alarmCfg.ringing = false;
  alarmCfg.snoozed = false;
}
