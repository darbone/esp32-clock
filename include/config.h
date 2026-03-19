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
#define BL_PIN          27   // backlight PWM
#define BL_CHANNEL      0
#define BAT_ADC_PIN     34   // baca voltase 18650 lewat voltage divider

// ===== Lokasi buat Open-Meteo =====
// Salatiga, Jawa Tengah (ganti sesuai lokasi lo)
#define LATITUDE        "-7.3305"
#define LONGITUDE       "110.4982"
#define TIMEZONE        "Asia/Jakarta"

// ===== Auto Sleep =====
#define SLEEP_TIMEOUT_S 60    // detik sebelum sleep
#define DIM_BEFORE_S    10    // detik sebelum sleep mulai dim

// ===== Alarm default =====
#define ALARM_DEFAULT_H 7
#define ALARM_DEFAULT_M 0

// ===== NTP =====
#define NTP_SERVER      "pool.ntp.org"
#define NTP_OFFSET      25200  // UTC+7 (WIB) = 7*3600

// ===== Colors (RGB565) =====
#define C_BG            0x0821   // #060810
#define C_TIME          0xEF3F   // #e0f0ff
#define C_ACCENT        0x065F   // #00c8ff
#define C_RED           0xF800   // #ff0000
#define C_GREEN         0x07E0   // #00ff00
#define C_YELLOW        0xFFE0   // #ffff00
#define C_DIM           0x2945   // #2a4a6a
#define C_WHITE         0xFFFF
#define C_BLACK         0x0000
