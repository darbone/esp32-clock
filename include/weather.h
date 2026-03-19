#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

struct WeatherData {
  float temp        = 0.0;
  int   humidity    = 0;
  int   weatherCode = 0;
  bool  valid       = false;
};

const char* wmoToLabel(int code) {
  if (code == 0)              return "CERAH";
  if (code <= 2)              return "BERAWAN";
  if (code == 3)              return "MENDUNG";
  if (code >= 45 && code<=48) return "KABUT";
  if (code >= 51 && code<=65) return "HUJAN";
  if (code >= 80 && code<=82) return "SHOWER";
  if (code >= 95)             return "BADAI";
  return "N/A";
}

bool fetchWeather(WeatherData& wd) {
  if (WiFi.status() != WL_CONNECTED) return false;
  HTTPClient http;
  String url = "https://api.open-meteo.com/v1/forecast"
               "?latitude=" LATITUDE "&longitude=" LONGITUDE
               "&current=temperature_2m,relative_humidity_2m,weather_code"
               "&timezone=" TIMEZONE "&forecast_days=1";
  http.begin(url);
  http.setTimeout(8000);
  int code = http.GET();
  if (code != 200) { http.end(); return false; }
  String payload = http.getString();
  http.end();
  JsonDocument doc;
  if (deserializeJson(doc, payload)) return false;
  JsonObject cur = doc["current"];
  wd.temp        = cur["temperature_2m"].as<float>();
  wd.humidity    = cur["relative_humidity_2m"].as<int>();
  wd.weatherCode = cur["weather_code"].as<int>();
  wd.valid       = true;
  return true;
}
