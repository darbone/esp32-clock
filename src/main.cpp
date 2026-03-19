#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUDP.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "weather.h"
#include "battery.h"
#include "alarm.h"
#include "sleep.h"
#include "display.h"
#include "menu.h"

// ===== OBJECTS =====
TFT_eSPI   tft;
WiFiUDP    ntpUDP;
NTPClient  ntpClient(ntpUDP, NTP_SERVER, NTP_OFFSET, 60000);

// ===== STATE =====
int brightness   = 200;
int sleepTimeout = SLEEP_TIMEOUT_S;
int buzzerMode   = 0;

// ===== TIMING =====
unsigned long lastClockDraw    = 0;
unsigned long lastWeatherFetch = 0;
unsigned long lastBatRead      = 0;
unsigned long lastTopBar       = 0;
bool          colonState       = true;

// ===== WEATHER CACHE =====
WeatherData wd;

// Forecast dummy (ganti dengan fetch real kalau mau)
float fTemps[]    = {28, 26, 24, 22};
int   fCodes[]    = {0, 1, 61, 95};
const char* fDays[] = {"JUM","SAB","MIN","SEN"};

// ===== BUTTONS =====
struct Btn { int pin; bool last; unsigned long debounce; };
Btn btns[] = {
  {BTN_UP,   HIGH, 0},
  {BTN_DOWN, HIGH, 0},
  {BTN_MENU, HIGH, 0},
  {BTN_SEL,  HIGH, 0},
};

bool btnPressed(int i) {
  bool cur = digitalRead(btns[i].pin);
  if (cur == LOW && btns[i].last == HIGH) {
    unsigned long now = millis();
    if (now - btns[i].debounce > BTN_DEBOUNCE_MS) {
      btns[i].debounce = now;
      btns[i].last = cur;
      return true;
    }
  }
  btns[i].last = cur;
  return false;
}

// ===== DRAW MAIN =====
void drawMain() {
  time_t epoch = ntpClient.getEpochTime();
  struct tm* t = localtime(&epoch);

  int h = t->tm_hour;
  int m = t->tm_min;

  if (!fmt24h) h = (h % 12 == 0) ? 12 : h % 12;

  drawClock(h, m, colonState);
  drawRightPanel(
    wd.valid ? wd.temp : 0.0f,
    t->tm_mday, t->tm_mon + 1, t->tm_wday
  );
}

void drawTopBarFull() {
  drawTopBar(
    wd.valid ? wmoToLabel(wd.weatherCode) : "...",
    WiFi.RSSI(), brightness, batPercent,
    alarmCfg.enabled, alarmCfg.hour, alarmCfg.minute
  );
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  // Backlight
  ledcSetup(BL_CHANNEL, 5000, 8);
  ledcAttachPin(BL_PIN, BL_CHANNEL);
  ledcWrite(BL_CHANNEL, 50); // dim saat boot

  // TFT init
  tft.init();
  tft.setRotation(1); // landscape 320x240
  tft.fillScreen(C_BG);
  tft.setSwapBytes(true);

  // Splash screen
  tft.setTextColor(C_ACCENT, C_BG);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("ESP32  CLOCK", 160, 90, 4);
  tft.setTextColor(0x2945, C_BG);
  tft.drawString("PAK KARDI NOBO", 160, 120, 2);
  ledcWrite(BL_CHANNEL, brightness);

  // Buttons
  for (auto& b : btns) pinMode(b.pin, INPUT_PULLUP);

  // WiFi
  tft.setTextColor(0x2945, C_BG);
  tft.drawString("Connecting WiFi...", 160, 155, 1);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 30) {
    delay(500); retry++;
    tft.drawString(String(retry), 160 + retry * 4, 170, 1);
  }

  if (WiFi.status() == WL_CONNECTED) {
    tft.setTextColor(C_GREEN, C_BG);
    tft.drawString("WiFi OK!", 160, 185, 2);
    ntpClient.begin();
    ntpClient.forceUpdate();
    delay(500);
    fetchWeather(wd);
  } else {
    tft.setTextColor(C_RED, C_BG);
    tft.drawString("WiFi GAGAL", 160, 185, 2);
    delay(1000);
  }

  // Load alarm
  alarmLoad();

  // ADC baterai
  pinMode(BAT_ADC_PIN, INPUT);
  analogSetAttenuation(ADC_11db);
  batRead();

  delay(800);
  clearScreen();
  resetSleepTimer();

  // Draw awal
  drawTopBarFull();
  drawMain();
  drawForecast(fTemps, fCodes, fDays, 4);
}

// ===== LOOP =====
void loop() {
  unsigned long now = millis();
  bool anyBtn = false;

  // ===== BUTTONS =====
  if (btnPressed(0)) { // UP
    anyBtn = true;
    if (isSleeping) { wakeUp(BL_CHANNEL, brightness); clearScreen(); drawTopBarFull(); drawMain(); drawForecast(fTemps, fCodes, fDays, 4); }
    else if (menuPage != MENU_CLOSED) menuHandleUp();
    else { brightness = min(255, brightness + 20); ledcWrite(BL_CHANNEL, brightness); }
  }
  if (btnPressed(1)) { // DOWN
    anyBtn = true;
    if (isSleeping) { wakeUp(BL_CHANNEL, brightness); clearScreen(); drawTopBarFull(); drawMain(); drawForecast(fTemps, fCodes, fDays, 4); }
    else if (menuPage != MENU_CLOSED) menuHandleDown();
    else { brightness = max(10, brightness - 20); ledcWrite(BL_CHANNEL, brightness); }
  }
  if (btnPressed(2)) { // MENU
    anyBtn = true;
    if (isSleeping) { wakeUp(BL_CHANNEL, brightness); clearScreen(); drawTopBarFull(); drawMain(); drawForecast(fTemps, fCodes, fDays, 4); }
    else if (alarmCfg.ringing) snoozeAlarm();
    else {
      if (menuPage == MENU_CLOSED) menuOpen();
      else menuHandleMenu();
      if (menuPage == MENU_CLOSED) { clearScreen(); drawTopBarFull(); drawMain(); drawForecast(fTemps, fCodes, fDays, 4); }
    }
  }
  if (btnPressed(3)) { // SELECT
    anyBtn = true;
    if (isSleeping) { wakeUp(BL_CHANNEL, brightness); clearScreen(); drawTopBarFull(); drawMain(); drawForecast(fTemps, fCodes, fDays, 4); }
    else if (alarmCfg.ringing) dismissAlarm();
    else if (menuPage != MENU_CLOSED) menuHandleSelect();
  }

  if (anyBtn) resetSleepTimer();

  // ===== SLEEP =====
  if (menuPage == MENU_CLOSED) {
    bool justSlept = checkAutoSleep(BL_CHANNEL, brightness);
    if (justSlept) tft.fillScreen(TFT_BLACK);
    if (isSleeping) { delay(50); return; }
  }

  // ===== CLOCK UPDATE setiap 500ms =====
  if (menuPage == MENU_CLOSED && now - lastClockDraw > 500) {
    lastClockDraw = now;
    colonState = !colonState;
    ntpClient.update();
    drawMain();
  }

  // ===== TOP BAR update setiap 5 detik =====
  if (menuPage == MENU_CLOSED && now - lastTopBar > 5000) {
    lastTopBar = now;
    drawTopBarFull();
  }

  // ===== BATERAI setiap 30 detik =====
  if (now - lastBatRead > 30000) {
    lastBatRead = now;
    batRead();
  }

  // ===== CUACA setiap 10 menit =====
  if (now - lastWeatherFetch > 600000UL || lastWeatherFetch == 0) {
    lastWeatherFetch = now;
    if (WiFi.status() == WL_CONNECTED) fetchWeather(wd);
  }

  // ===== ALARM =====
  if (menuPage == MENU_CLOSED) {
    time_t epoch = ntpClient.getEpochTime();
    struct tm* t = localtime(&epoch);
    checkAlarm(BUZZER_PIN, t->tm_hour, t->tm_min, buzzerMode);
  }

  delay(20);
}
