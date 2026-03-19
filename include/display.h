#pragma once
#include <TFT_eSPI.h>
#include "config.h"
#include "weather.h"
#include "battery.h"

extern TFT_eSPI tft;

// ===== TOP BAR =====
void drawTopBar(const char* weatherLabel, int wifiRSSI,
                int bright, int bat, bool alarmOn,
                int alarmH, int alarmM) {

  tft.fillRect(0, 0, 320, 18, C_BG);

  // Cuaca kiri
  tft.setTextColor(0x5BDF, C_BG);
  tft.setTextDatum(ML_DATUM);
  tft.setTextSize(1);
  tft.drawString(weatherLabel, 4, 9, 2);

  // WiFi bars
  int wx = 118;
  int bars = 0;
  if      (wifiRSSI >= -50) bars = 4;
  else if (wifiRSSI >= -60) bars = 3;
  else if (wifiRSSI >= -70) bars = 2;
  else if (wifiRSSI >= -80) bars = 1;
  for (int i = 0; i < 4; i++) {
    int h = 3 + i * 3;
    uint16_t c = (i < bars) ? C_ACCENT : 0x2945;
    tft.fillRect(wx + i*5, 16-h, 3, h, c);
  }

  // Brightness bar
  tft.setTextColor(0x2945, C_BG);
  tft.drawString("B", 144, 9, 1);
  tft.drawRect(152, 5, 28, 7, 0x2945);
  tft.fillRect(153, 6, map(bright, 0, 255, 0, 26), 5, C_ACCENT);

  // Battery
  int bx = 186;
  tft.drawRect(bx, 4, 22, 10, 0x2945);
  tft.fillRect(bx+22, 7, 2, 4, 0x2945);
  uint16_t bc = (bat > 50) ? 0x07E0 : (bat > 20) ? 0xFFE0 : C_RED;
  tft.fillRect(bx+1, 5, map(bat,0,100,0,20), 8, bc);

  // Alarm
  if (alarmOn) {
    char buf[12];
    sprintf(buf, "AL %02d:%02d", alarmH, alarmM);
    tft.setTextColor(C_RED, C_BG);
    tft.setTextDatum(MR_DATUM);
    tft.drawString(buf, 318, 9, 2);
  }

  // Garis bawah topbar
  tft.drawFastHLine(0, 18, 320, 0x1082);
}

// ===== JAM GEDE =====
// Font 7 = 7-segment, tinggi ~48px per karakter di size 1
void drawClock(int h, int m, bool colonOn) {
  // Clear area jam
  tft.fillRect(0, 20, 254, 158, C_BG);

  char hStr[3], mStr[3];
  sprintf(hStr, "%02d", h);
  sprintf(mStr, "%02d", m);

  tft.setTextDatum(ML_DATUM);

  // Jam kiri — Font 7 (7-segment bawaan TFT_eSPI)
  tft.setTextColor(C_TIME, C_BG);
  tft.drawString(hStr, 4, 100, 7);

  // Titik dua — blink
  tft.setTextColor(colonOn ? C_ACCENT : C_BG, C_BG);
  tft.drawString(":", 116, 92, 7);

  // Menit kanan
  tft.setTextColor(C_TIME, C_BG);
  tft.drawString(mStr, 134, 100, 7);

  // Garis kanan pemisah panel
  tft.drawFastVLine(255, 19, 160, 0x1082);
}

// ===== PANEL KANAN: suhu + tanggal =====
void drawRightPanel(float temp, int day, int month, int dow) {
  const char* months[] = {"JAN","FEB","MAR","APR","MEI","JUN",
                           "JUL","AGU","SEP","OKT","NOV","DES"};
  const char* dows[]   = {"MIN","SEN","SEL","RAB","KAM","JUM","SAB"};

  tft.fillRect(256, 19, 64, 160, C_BG);

  tft.setTextDatum(MC_DATUM);

  // Suhu
  tft.setTextColor(C_ACCENT, C_BG);
  char tmpStr[8];
  sprintf(tmpStr, "%.1f", temp);
  tft.drawString(tmpStr, 288, 45, 4);

  tft.setTextColor(0x4A8A, C_BG);
  tft.setTextSize(1);
  tft.drawString("o C", 288, 62, 1);

  // Divider
  tft.drawFastHLine(260, 78, 56, 0x1082);

  // Bulan kecil
  tft.setTextColor(0x5BDF, C_BG);
  tft.drawString(months[month-1], 288, 92, 2);

  // Tanggal GEDE
  tft.setTextColor(C_ACCENT, C_BG);
  char dayStr[3];
  sprintf(dayStr, "%02d", day);
  tft.drawString(dayStr, 288, 130, 6);

  // Hari
  tft.setTextColor(0x2945, C_BG);
  tft.drawString(dows[dow], 288, 162, 1);
}

// ===== FORECAST ROW =====
void drawForecast(float temps[], int codes[], const char* days[], int count) {
  tft.fillRect(0, 180, 320, 60, C_BG);
  tft.drawFastHLine(0, 180, 320, 0x1082);

  int fw = 320 / count;
  for (int i = 0; i < count; i++) {
    int x = i * fw;
    if (i > 0) tft.drawFastVLine(x, 181, 59, 0x1082);

    tft.setTextDatum(MC_DATUM);

    // Hari
    tft.setTextColor(0x2945, C_BG);
    tft.drawString(days[i], x + fw/2, 191, 1);

    // Label cuaca
    tft.setTextColor(0x3A6A, C_BG);
    tft.drawString(wmoToLabel(codes[i]), x + fw/2, 206, 1);

    // Suhu
    tft.setTextColor(C_ACCENT, C_BG);
    char t[6];
    sprintf(t, "%d`", (int)temps[i]);
    tft.drawString(t, x + fw/2, 224, 2);
  }
}

void clearScreen() {
  tft.fillScreen(C_BG);
}
