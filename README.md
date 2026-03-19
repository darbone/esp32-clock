# ESP32 Weather Clock — PlatformIO

## Cara Upload ke VS Code

### 1. Install VS Code + PlatformIO
- Download VS Code: https://code.visualstudio.com
- Buka VS Code → Extensions (Ctrl+Shift+X)
- Cari **PlatformIO IDE** → Install
- Tunggu install selesai, restart VS Code

### 2. Buka Project
- Buka VS Code
- Klik **PlatformIO icon** (semut di sidebar kiri)
- Pilih **Open Project**
- Arahkan ke folder `esp32_clock` ini

### 3. Wiring ESP32 ↔ ILI9341
```
ILI9341     ESP32
-------     -----
VCC    →    3.3V
GND    →    GND
CS     →    GPIO 5
RESET  →    GPIO 4
DC     →    GPIO 2
MOSI   →    GPIO 23
SCK    →    GPIO 18
LED    →    GPIO 27  (via 100Ω resistor)
MISO   →    GPIO 19  (opsional)
```

### 4. Wiring Tombol
```
BTN_UP    → GPIO 12 → GND  (pakai PULLUP internal)
BTN_DOWN  → GPIO 13 → GND
BTN_MENU  → GPIO 14 → GND
BTN_SEL   → GPIO 15 → GND
```

### 5. Wiring Buzzer PC
```
BUZZER (+) → GPIO 25
BUZZER (-) → GND
```
> Buzzer PC (pasif) langsung colok, no resistor needed

### 6. Wiring Baterai 18650
```
18650 (+) → Voltage Divider → GPIO 34
           100kΩ ke VBAT
           100kΩ ke GND
18650 (-) → GND

Atau pakai TP4056 modul charger + output ke ESP32 VIN
```

### 7. Upload
- Colok ESP32 ke PC via USB
- Di VS Code bawah klik **→ Upload** (atau Ctrl+Alt+U)
- Tunggu compile + upload selesai
- Done! 🎉

### 8. Ganti Lokasi (buat cuaca)
Edit `include/config.h`:
```cpp
#define LATITUDE   "-7.3305"   // ganti koordinat lo
#define LONGITUDE  "110.4982"
```
Cari koordinat di: https://www.latlong.net

### Troubleshoot
- **Upload gagal**: Tahan tombol BOOT di ESP32 saat upload
- **Layar putih**: Cek wiring SPI, pastiin CS/DC/RST bener
- **WiFi gagal**: Pastiin SSID & password bener di config.h
- **Cuaca ga muncul**: Cek Serial Monitor (115200 baud) buat debug
