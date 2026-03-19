#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

struct AlarmConfig {
  int  hour    = ALARM_DEFAULT_H;
  int  minute  = ALARM_DEFAULT_M;
  bool enabled = true;
  bool ringing = false;
  bool snoozed = false;
  unsigned long snoozeUntil = 0;
};

AlarmConfig alarmCfg;
Preferences  prefs;

void alarmSave() {
  prefs.begin("alarm", false);
  prefs.putInt("h", alarmCfg.hour);
  prefs.putInt("m", alarmCfg.minute);
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

void buzzTone(int pin, int freq, int dur) {
  ledcSetup(2, freq, 8);
  ledcAttachPin(pin, 2);
  ledcWrite(2, 128);
  delay(dur);
  ledcWrite(2, 0);
}

void playBeep(int pin) {
  int seq[][2] = {{2000,150},{0,80},{2000,150},{0,80},{2500,350},{0,400}};
  for (auto& s : seq) {
    if (s[0] > 0) buzzTone(pin, s[0], s[1]);
    else delay(s[1]);
  }
}

void checkAlarm(int pin, int h, int m) {
  if (!alarmCfg.enabled || alarmCfg.snoozed) return;
  if (alarmCfg.snoozed && millis() > alarmCfg.snoozeUntil)
    alarmCfg.snoozed = false;
  if (h == alarmCfg.hour && m == alarmCfg.minute)
    alarmCfg.ringing = true;
  if (alarmCfg.ringing) playBeep(pin);
}

void snoozeAlarm()  {
  alarmCfg.ringing   = false;
  alarmCfg.snoozed   = true;
  alarmCfg.snoozeUntil = millis() + 5UL * 60000UL;
}

void dismissAlarm() {
  alarmCfg.ringing = false;
  alarmCfg.snoozed = false;
}
