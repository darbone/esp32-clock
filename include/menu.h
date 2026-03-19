#pragma once
#include <TFT_eSPI.h>
#include "config.h"
#include "alarm.h"

extern TFT_eSPI tft;
extern int brightness;
extern int sleepTimeout;
extern int buzzerMode;

enum MenuPage { MENU_CLOSED, MENU_MAIN, MENU_BRIGHT, MENU_SLEEP, MENU_ALARM, MENU_FORMAT };

MenuPage menuPage = MENU_CLOSED;
int      menuIdx  = 0;
bool     fmt24h   = true;

const char* menuItems[] = {
  "KECERAHAN",
  "AUTO SLEEP",
  "ALARM",
  "FORMAT JAM",
};
const int MENU_COUNT = 4;

void drawMenuMain() {
  tft.fillScreen(C_BG);
  tft.setTextColor(C_ACCENT, C_BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("== PENGATURAN ==", 160, 20, 2);
  for (int i = 0; i < MENU_COUNT; i++) {
    bool sel = (i == menuIdx);
    tft.fillRect(20, 45 + i * 40, 280, 34, sel ? 0x0839 : C_BG);
    tft.drawRect(20, 45 + i * 40, 280, 34, sel ? C_ACCENT : C_DIM);
    tft.setTextColor(sel ? C_ACCENT : C_DIM, sel ? 0x0839 : C_BG);
    tft.setTextDatum(ML_DATUM);
    tft.drawString(menuItems[i], 36, 62 + i * 40, 2);
  }
  tft.setTextColor(C_DIM, C_BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("UP/DN=PILIH  SEL=MASUK  MENU=KELUAR", 160, 228, 1);
}

void drawMenuBright() {
  tft.fillScreen(C_BG);
  tft.setTextColor(C_ACCENT, C_BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("KECERAHAN", 160, 30, 4);
  int pct = map(brightness, 0, 255, 0, 100);
  tft.drawRect(20, 100, 280, 20, C_DIM);
  tft.fillRect(21, 101, map(pct, 0, 100, 0, 278), 18, C_ACCENT);
  tft.setTextColor(C_TIME, C_BG);
  tft.drawString(String(pct) + "%", 160, 150, 4);
  tft.setTextColor(C_DIM, C_BG);
  tft.drawString("UP=+ DOWN=-  MENU=KEMBALI", 160, 210, 1);
}

void drawMenuSleep() {
  tft.fillScreen(C_BG);
  tft.setTextColor(C_ACCENT, C_BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("AUTO SLEEP", 160, 30, 4);
  tft.setTextColor(C_TIME, C_BG);
  tft.drawString(String(sleepTimeout) + " DETIK", 160, 120, 4);
  tft.setTextColor(C_DIM, C_BG);
  tft.drawString("UP=+ DOWN=-  MENU=KEMBALI", 160, 210, 1);
}

void drawMenuAlarm() {
  tft.fillScreen(C_BG);
  tft.setTextColor(C_ACCENT, C_BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("ALARM", 160, 30, 4);
  char buf[6];
  sprintf(buf, "%02d:%02d", alarmCfg.hour, alarmCfg.minute);
  tft.setTextColor(alarmCfg.enabled ? C_TIME : C_DIM, C_BG);
  tft.drawString(buf, 160, 110, 7);
  tft.setTextColor(alarmCfg.enabled ? C_GREEN : C_RED, C_BG);
  tft.drawString(alarmCfg.enabled ? "ON" : "OFF", 160, 170, 4);
  tft.setTextColor(C_DIM, C_BG);
  tft.drawString("UP=JAM+  DN=JAM-  SEL=ON/OFF", 160, 205, 1);
  tft.drawString("MENU=KEMBALI", 160, 218, 1);
}

void drawMenuFormat() {
  tft.fillScreen(C_BG);
  tft.setTextColor(C_ACCENT, C_BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("FORMAT JAM", 160, 30, 4);
  tft.fillRect(30,  80, 110, 60, fmt24h  ? 0x0839 : C_BG);
  tft.drawRect(30,  80, 110, 60, fmt24h  ? C_ACCENT : C_DIM);
  tft.setTextColor(fmt24h  ? C_ACCENT : C_DIM, fmt24h  ? 0x0839 : C_BG);
  tft.drawString("24 JAM", 85, 110, 2);
  tft.fillRect(180, 80, 110, 60, !fmt24h ? 0x0839 : C_BG);
  tft.drawRect(180, 80, 110, 60, !fmt24h ? C_ACCENT : C_DIM);
  tft.setTextColor(!fmt24h ? C_ACCENT : C_DIM, !fmt24h ? 0x0839 : C_BG);
  tft.drawString("12 JAM", 235, 110, 2);
  tft.setTextColor(C_DIM, C_BG);
  tft.drawString("UP/DN=GANTI  MENU=KEMBALI", 160, 210, 1);
}

void menuHandleUp() {
  switch (menuPage) {
    case MENU_MAIN:
      menuIdx = (menuIdx - 1 + MENU_COUNT) % MENU_COUNT;
      drawMenuMain(); break;
    case MENU_BRIGHT:
      brightness = min(255, brightness + 15);
      ledcWrite(BL_CHANNEL, brightness);
      drawMenuBright(); break;
    case MENU_SLEEP:
      sleepTimeout = min(300, sleepTimeout + 10);
      drawMenuSleep(); break;
    case MENU_ALARM:
      alarmCfg.hour = (alarmCfg.hour + 1) % 24;
      drawMenuAlarm(); break;
    case MENU_FORMAT:
      fmt24h = !fmt24h;
      drawMenuFormat(); break;
    default: break;
  }
}

void menuHandleDown() {
  switch (menuPage) {
    case MENU_MAIN:
      menuIdx = (menuIdx + 1) % MENU_COUNT;
      drawMenuMain(); break;
    case MENU_BRIGHT:
      brightness = max(10, brightness - 15);
      ledcWrite(BL_CHANNEL, brightness);
      drawMenuBright(); break;
    case MENU_SLEEP:
      sleepTimeout = max(10, sleepTimeout - 10);
      drawMenuSleep(); break;
    case MENU_ALARM:
      alarmCfg.hour = (alarmCfg.hour - 1 + 24) % 24;
      drawMenuAlarm(); break;
    case MENU_FORMAT:
      fmt24h = !fmt24h;
      drawMenuFormat(); break;
    default: break;
  }
}

void menuHandleSelect() {
  switch (menuPage) {
    case MENU_MAIN:
      menuPage = (MenuPage)(menuIdx + 2);
      switch (menuPage) {
        case MENU_BRIGHT: drawMenuBright(); break;
        case MENU_SLEEP:  drawMenuSleep();  break;
        case MENU_ALARM:  drawMenuAlarm();  break;
        case MENU_FORMAT: drawMenuFormat(); break;
        default: break;
      }
      break;
    case MENU_ALARM:
      alarmCfg.enabled = !alarmCfg.enabled;
      alarmSave();
      drawMenuAlarm();
      break;
    default: break;
  }
}

void menuHandleMenu() {
  if (menuPage == MENU_MAIN) {
    menuPage = MENU_CLOSED;
  } else if (menuPage != MENU_CLOSED) {
    menuPage = MENU_MAIN;
    drawMenuMain();
  }
}

void menuOpen() {
  menuPage = MENU_MAIN;
  menuIdx  = 0;
  drawMenuMain();
}
