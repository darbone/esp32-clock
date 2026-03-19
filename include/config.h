#pragma once

// ===== WiFi =====
#define WIFI_SSID       "PAK KARDI NOBO"
#define WIFI_PASS       "12345679"

// ===== Buttons =====
#define BTN_UP          12
#define BTN_DOWN        13
#define BTN_MENU        14
#define BTN_SEL         15
#define BTN_DEBOUNCE_MS 200

// ===== Hardware =====
#define BUZZER_PIN      25
#define BL_PIN          27
#define BL_CHANNEL      0
#define BAT_ADC_PIN     34

// ===== Lokasi Open-Meteo (Salatiga) =====
#define LATITUDE        "-7.3305"
#define LONGITUDE       "110.4982"
#define TIMEZONE        "Asia/Jakarta"

// ===== NTP =====
#define NTP_SERVER      "pool.ntp.org"
#define NTP_OFFSET      25200   // UTC+7

// ===== Sleep =====
#define SLEEP_TIMEOUT_S  60
#define DIM_BEFORE_S     10

// ===== Alarm =====
#define ALARM_DEFAULT_H  7
#define ALARM_DEFAULT_M  0

// ===== LVGL display buffer =====
// 1/10 layar: 320 * 24 pixels
#define LV_BUF_SIZE     (320 * 24)

// ===== Colors LVGL (lv_color_make) =====
#define CLR_BG          lv_color_make(6,   8,  16)
#define CLR_TIME        lv_color_make(221, 238, 255)
#define CLR_ACCENT      lv_color_make(0,   200, 255)
#define CLR_ACCENT2     lv_color_make(0,   170, 220)
#define CLR_DIM         lv_color_make(42,  74,  106)
#define CLR_RED         lv_color_make(255, 50,  50)
#define CLR_GREEN       lv_color_make(0,   255, 136)
#define CLR_YELLOW      lv_color_make(255, 214, 0)
#define CLR_PANEL       lv_color_make(8,   12,  20)
#define CLR_LINE        lv_color_make(13,  32,  48)
