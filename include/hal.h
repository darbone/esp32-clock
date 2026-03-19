#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>
#include "config.h"

TFT_eSPI tft;

// LVGL display buffer — 1/10 layar
static lv_disp_draw_buf_t draw_buf;
static lv_color_t         buf1[LV_BUF_SIZE];
static lv_color_t         buf2[LV_BUF_SIZE]; // double buffer

// ===== LVGL flush callback =====
void lvgl_flush(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p) {
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t*)color_p, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(drv);
}

// ===== Init HAL =====
void hal_init() {
  // Backlight PWM
  ledcSetup(BL_CHANNEL, 5000, 8);
  ledcAttachPin(BL_PIN, BL_CHANNEL);
  ledcWrite(BL_CHANNEL, 0);

  // TFT
  tft.init();
  tft.setRotation(1); // landscape 320x240
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);

  // LVGL init
  lv_init();

  // Setup display buffer
  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, LV_BUF_SIZE);

  // Register display driver
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res  = 320;
  disp_drv.ver_res  = 240;
  disp_drv.flush_cb = lvgl_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  ledcWrite(BL_CHANNEL, 200); // backlight on
}
