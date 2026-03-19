/**
 * lv_conf.h — LVGL 8.x config untuk ESP32 + ILI9341
 * Taruh di: include/lv_conf.h
 */

#if 1 /* Set to "1" to enable content */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/
#define LV_COLOR_DEPTH     16   /* RGB565 untuk ILI9341 */
#define LV_COLOR_16_SWAP   1    /* ESP32 SPI butuh swap bytes */

/*====================
   MEMORY SETTINGS
 *====================*/
/* LVGL internal heap — jangan terlalu besar buat ESP32 biasa */
#define LV_MEM_CUSTOM      0
#define LV_MEM_SIZE        (48 * 1024U)  /* 48KB cukup buat UI kita */

/*====================
   HAL SETTINGS
 *====================*/
#define LV_TICK_CUSTOM     1
#define LV_TICK_CUSTOM_INCLUDE  "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())

/*====================
   DISPLAY BUFFER
 *====================*/
/* 1/10 layar = 320*24 = 7680 px = 15360 bytes, aman buat ESP32 */
#define LV_HOR_RES_MAX     320
#define LV_VER_RES_MAX     240

/*====================
   FONT
 *====================*/
/* Font bawaan yang dipakai */
#define LV_FONT_MONTSERRAT_8   1
#define LV_FONT_MONTSERRAT_12  1
#define LV_FONT_MONTSERRAT_14  1
#define LV_FONT_MONTSERRAT_16  1
#define LV_FONT_MONTSERRAT_20  1
#define LV_FONT_MONTSERRAT_24  1
#define LV_FONT_MONTSERRAT_28  1
#define LV_FONT_MONTSERRAT_36  1
#define LV_FONT_MONTSERRAT_48  1

/* Font default */
#define LV_FONT_DEFAULT        &lv_font_montserrat_14

/* Enable font besar buat jam */
#define LV_FONT_FMT_TXT_LARGE  1

/*====================
   FEATURE ON/OFF
 *====================*/
#define LV_USE_PERF_MONITOR  0   /* Matiin FPS counter */
#define LV_USE_MEM_MONITOR   0
#define LV_USE_LOG           0   /* Matiin log (hemat RAM) */

/* Widget yang dipakai */
#define LV_USE_LABEL         1
#define LV_USE_BAR           1
#define LV_USE_BTN           1
#define LV_USE_CONT          1
#define LV_USE_LINE          1
#define LV_USE_IMG           0   /* Matiin image (ga dipakai) */
#define LV_USE_LIST          0
#define LV_USE_TABLE         0
#define LV_USE_CHART         0
#define LV_USE_MSGBOX        0
#define LV_USE_SPINNER       0
#define LV_USE_KEYBOARD      0
#define LV_USE_TEXTAREA      0
#define LV_USE_CALENDAR      0
#define LV_USE_COLORWHEEL    0
#define LV_USE_LED           0
#define LV_USE_METER         0
#define LV_USE_ARC           0
#define LV_USE_CHECKBOX      0
#define LV_USE_DROPDOWN      0
#define LV_USE_ROLLER        0
#define LV_USE_SLIDER        1   /* Buat brightness di menu */
#define LV_USE_SWITCH        1   /* Buat on/off alarm */

/* Animation */
#define LV_USE_ANIMATION     1

/* Theme */
#define LV_USE_THEME_DEFAULT 1
#define LV_THEME_DEFAULT_DARK 1  /* Dark mode */

#endif /* LV_CONF_H */
#endif /* End of "Content enable" */
