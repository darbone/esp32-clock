#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUDP.h>
#include <lvgl.h>
#include "config.h"
#include "hal.h"
#include "ui.h"
#include "weather.h"
#include "battery.h"
#include "alarm.h"

// ===== NTP =====
WiFiUDP   ntpUDP;
NTPClient ntpClient(ntpUDP, NTP_SERVER, NTP_OFFSET, 60000);

// ===== STATE =====
int  brightness   = 200;
int  sleepTimeout = SLEEP_TIMEOUT_S;
bool fmt24h       = true;
bool isSleeping   = false;
bool menuOpen     = false;

// ===== TIMING =====
unsigned long lastClock   = 0;
unsigned long lastWeather = 0;
unsigned long lastBat     = 0;
unsigned long lastActivity= 0;
bool          colonState  = true;

WeatherData wd;

// ===== BUTTONS =====
struct Btn { int pin; bool last; unsigned long deb; };
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
    if (now - btns[i].deb > BTN_DEBOUNCE_MS) {
      btns[i].deb  = now;
      btns[i].last = cur;
      return true;
    }
  }
  btns[i].last = cur;
  return false;
}

void resetActivity() {
  lastActivity = millis();
  if (isSleeping) {
    isSleeping = false;
    ledcWrite(BL_CHANNEL, brightness);
    ui_show_clock();
    lv_timer_handler();
  }
}

// ===== MENU ACTION =====
void menuAction(int idx) {
  char buf[16];
  switch (idx) {
    case 0: // Kecerahan
      brightness = (brightness >= 240) ? 50 : brightness + 50;
      ledcWrite(BL_CHANNEL, brightness);
      snprintf(buf, 16, "%d%%", map(brightness, 0, 255, 0, 100));
      ui_update_brightness(map(brightness, 0, 255, 0, 100));
      ui_menu_set_val(0, buf);
      break;
    case 1: // Sleep timeout
      sleepTimeout = (sleepTimeout >= 120) ? 10 : sleepTimeout + 10;
      snprintf(buf, 16, "%ds", sleepTimeout);
      ui_menu_set_val(1, buf);
      break;
    case 2: // Alarm jam +1
      alarmCfg.hour = (alarmCfg.hour + 1) % 24;
      alarmSave();
      snprintf(buf, 16, "%02d:%02d", alarmCfg.hour, alarmCfg.minute);
      ui_menu_set_val(2, buf);
      ui_update_alarm(alarmCfg.enabled, alarmCfg.hour, alarmCfg.minute);
      break;
    case 3: // Format 24/12
      fmt24h = !fmt24h;
      ui_menu_set_val(3, fmt24h ? "24H" : "12H");
      break;
    case 4: // WiFi status
      ui_menu_set_val(4, WiFi.status() == WL_CONNECTED ? "OK" : "FAIL");
      break;
  }
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  // Buttons
  for (auto& b : btns) pinMode(b.pin, INPUT_PULLUP);

  // HAL (TFT + LVGL)
  hal_init();

  // Splash
  lv_obj_t* splash = lv_obj_create(lv_scr_act());
  lv_obj_set_size(splash, 320, 240);
  lv_obj_set_style_bg_color(splash, lv_color_make(6,8,16), 0);
  lv_obj_t* s1 = lv_label_create(splash);
  lv_obj_set_style_text_color(s1, lv_color_make(0,200,255), 0);
  lv_obj_set_style_text_font(s1, &lv_font_montserrat_20, 0);
  lv_obj_align(s1, LV_ALIGN_CENTER, 0, -20);
  lv_label_set_text(s1, "ESP32 CLOCK");
  lv_obj_t* s2 = lv_label_create(splash);
  lv_obj_set_style_text_color(s2, lv_color_make(42,74,106), 0);
  lv_obj_set_style_text_font(s2, &lv_font_montserrat_14, 0);
  lv_obj_align(s2, LV_ALIGN_CENTER, 0, 10);
  lv_label_set_text(s2, "PAK KARDI NOBO");
  lv_timer_handler();

  // WiFi
  lv_obj_t* s3 = lv_label_create(splash);
  lv_obj_set_style_text_color(s3, lv_color_make(42,74,106), 0);
  lv_obj_set_style_text_font(s3, &lv_font_montserrat_12, 0);
  lv_obj_align(s3, LV_ALIGN_CENTER, 0, 40);
  lv_label_set_text(s3, "Connecting WiFi...");
  lv_timer_handler();

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500); retry++;
    lv_timer_handler();
  }

  if (WiFi.status() == WL_CONNECTED) {
    lv_label_set_text(s3, "WiFi OK!");
    lv_obj_set_style_text_color(s3, lv_color_make(0,200,100), 0);
    ntpClient.begin();
    ntpClient.forceUpdate();
    fetchWeather(wd);
  } else {
    lv_label_set_text(s3, "WiFi GAGAL — Offline Mode");
    lv_obj_set_style_text_color(s3, lv_color_make(255,50,50), 0);
  }
  lv_timer_handler();

  // Load alarm
  alarmLoad();

  // ADC baterai
  pinMode(BAT_ADC_PIN, INPUT);
  analogSetAttenuation(ADC_11db);
  batRead();

  delay(1200);

  // Build UI
  ui_init();
  ui_update_weather(wd.valid ? wmoToLabel(wd.weatherCode) : "OFFLINE");
  ui_update_temp(wd.valid ? wd.temp : 0);
  ui_update_battery(batPercent);
  ui_update_alarm(alarmCfg.enabled, alarmCfg.hour, alarmCfg.minute);

  lastActivity = millis();
}

// ===== LOOP =====
void loop() {
  unsigned long now = millis();

  // LVGL tick wajib dipanggil
  lv_timer_handler();

  // ===== BUTTONS =====
  if (btnPressed(0)) { // UP
    resetActivity();
    if (menuOpen) ui_menu_up();
    else { brightness = min(255, brightness+20); ledcWrite(BL_CHANNEL, brightness); ui_update_brightness(map(brightness,0,255,0,100)); }
  }
  if (btnPressed(1)) { // DOWN
    resetActivity();
    if (menuOpen) ui_menu_down();
    else { brightness = max(10, brightness-20); ledcWrite(BL_CHANNEL, brightness); ui_update_brightness(map(brightness,0,255,0,100)); }
  }
  if (btnPressed(2)) { // MENU
    resetActivity();
    if (alarmCfg.ringing) { snoozeAlarm(); return; }
    menuOpen = !menuOpen;
    if (menuOpen) ui_show_menu();
    else { ui_show_clock(); }
  }
  if (btnPressed(3)) { // SELECT
    resetActivity();
    if (alarmCfg.ringing) { dismissAlarm(); return; }
    if (menuOpen) menuAction(ui_menu_idx());
  }

  // ===== AUTO SLEEP =====
  if (!menuOpen && !isSleeping) {
    unsigned long idle = now - lastActivity;
    if (idle > (unsigned long)(sleepTimeout - DIM_BEFORE_S) * 1000) {
      int dimVal = map(idle,
        (sleepTimeout - DIM_BEFORE_S)*1000UL,
        sleepTimeout*1000UL, brightness, 5);
      ledcWrite(BL_CHANNEL, constrain(dimVal, 5, brightness));
    }
    if (idle > (unsigned long)sleepTimeout * 1000) {
      isSleeping = true;
      ledcWrite(BL_CHANNEL, 0);
      ui_show_sleep();
    }
  }

  // ===== CLOCK UPDATE setiap 500ms =====
  if (now - lastClock > 500) {
    lastClock  = now;
    colonState = !colonState;
    ntpClient.update();
    time_t epoch = ntpClient.getEpochTime();
    struct tm* t = localtime(&epoch);
    int h = t->tm_hour;
    if (!fmt24h) h = (h % 12 == 0) ? 12 : h % 12;
    ui_update_time(h, t->tm_min, colonState);
    ui_update_date(t->tm_mday, t->tm_mon+1, t->tm_wday);
    checkAlarm(BUZZER_PIN, t->tm_hour, t->tm_min);
  }

  // ===== BATERAI setiap 30 detik =====
  if (now - lastBat > 30000) {
    lastBat = now;
    batRead();
    ui_update_battery(batPercent);
  }

  // ===== CUACA setiap 10 menit =====
  if (now - lastWeather > 600000UL || lastWeather == 0) {
    lastWeather = now;
    if (WiFi.status() == WL_CONNECTED) {
      if (fetchWeather(wd)) {
        ui_update_weather(wmoToLabel(wd.weatherCode));
        ui_update_temp(wd.temp);
      }
    }
  }

  delay(5); // beri waktu LVGL bernafas
}
