#pragma once
#include <lvgl.h>
#include "config.h"

// ===== SCREENS =====
static lv_obj_t* scr_clock = NULL;
static lv_obj_t* scr_menu  = NULL;
static lv_obj_t* scr_sleep = NULL;

// ===== CLOCK SCREEN WIDGETS =====
static lv_obj_t* lbl_hour    = NULL;
static lv_obj_t* lbl_colon   = NULL;
static lv_obj_t* lbl_min     = NULL;
static lv_obj_t* lbl_temp    = NULL;
static lv_obj_t* lbl_month   = NULL;
static lv_obj_t* lbl_day     = NULL;
static lv_obj_t* lbl_dow     = NULL;
static lv_obj_t* lbl_weather = NULL;
static lv_obj_t* lbl_alarm   = NULL;
static lv_obj_t* bar_bright  = NULL;
static lv_obj_t* bar_bat     = NULL;
static lv_obj_t* lbl_bat_pct = NULL;

// Forecast labels
static lv_obj_t* lbl_fday[4];
static lv_obj_t* lbl_ftemp[4];

// ===== MENU WIDGETS =====
static lv_obj_t* menu_items[5];
static lv_obj_t* menu_vals[5];
static int        menu_idx = 0;
static const int  MENU_COUNT = 5;
static const char* menu_labels[] = {
  "KECERAHAN", "AUTO SLEEP", "ALARM", "FORMAT JAM", "WIFI"
};

// ===== STYLES =====
static lv_style_t style_bg;
static lv_style_t style_time;
static lv_style_t style_colon;
static lv_style_t style_accent;
static lv_style_t style_dim;
static lv_style_t style_small;
static lv_style_t style_menu_item;
static lv_style_t style_menu_sel;

// ===== FONT sizes =====
// LVGL built-in Montserrat
// 48px → jam (biggest available)
// 28px → tanggal, suhu
// 14px → label kecil
// 12px → topbar, forecast

void ui_styles_init() {
  // BG
  lv_style_init(&style_bg);
  lv_style_set_bg_color(&style_bg, CLR_BG);
  lv_style_set_bg_opa(&style_bg, LV_OPA_COVER);
  lv_style_set_border_width(&style_bg, 0);
  lv_style_set_pad_all(&style_bg, 0);
  lv_style_set_radius(&style_bg, 0);

  // Time (jam gede)
  lv_style_init(&style_time);
  lv_style_set_text_color(&style_time, CLR_TIME);
  lv_style_set_text_font(&style_time, &lv_font_montserrat_48);

  // Colon
  lv_style_init(&style_colon);
  lv_style_set_text_color(&style_colon, CLR_ACCENT);
  lv_style_set_text_font(&style_colon, &lv_font_montserrat_48);

  // Accent (suhu, tanggal)
  lv_style_init(&style_accent);
  lv_style_set_text_color(&style_accent, CLR_ACCENT);
  lv_style_set_text_font(&style_accent, &lv_font_montserrat_28);

  // Dim (label kecil)
  lv_style_init(&style_dim);
  lv_style_set_text_color(&style_dim, CLR_DIM);
  lv_style_set_text_font(&style_dim, &lv_font_montserrat_12);

  // Small topbar
  lv_style_init(&style_small);
  lv_style_set_text_color(&style_small, lv_color_make(90,170,208));
  lv_style_set_text_font(&style_small, &lv_font_montserrat_12);

  // Menu item normal
  lv_style_init(&style_menu_item);
  lv_style_set_bg_color(&style_menu_item, CLR_PANEL);
  lv_style_set_bg_opa(&style_menu_item, LV_OPA_COVER);
  lv_style_set_border_color(&style_menu_item, CLR_LINE);
  lv_style_set_border_width(&style_menu_item, 1);
  lv_style_set_radius(&style_menu_item, 3);
  lv_style_set_pad_all(&style_menu_item, 8);
  lv_style_set_text_color(&style_menu_item, CLR_DIM);
  lv_style_set_text_font(&style_menu_item, &lv_font_montserrat_14);

  // Menu item selected
  lv_style_init(&style_menu_sel);
  lv_style_set_bg_color(&style_menu_sel, lv_color_make(0,30,50));
  lv_style_set_border_color(&style_menu_sel, CLR_ACCENT);
  lv_style_set_border_width(&style_menu_sel, 1);
  lv_style_set_text_color(&style_menu_sel, CLR_ACCENT);
}

// ===== HELPER: line separator =====
lv_obj_t* make_hline(lv_obj_t* parent, int y, int x1, int x2) {
  lv_obj_t* line = lv_line_create(parent);
  static lv_point_t pts[2];
  pts[0] = {(lv_coord_t)x1, 0};
  pts[1] = {(lv_coord_t)x2, 0};
  lv_line_set_points(line, pts, 2);
  lv_obj_set_pos(line, 0, y);
  lv_obj_set_style_line_color(line, CLR_LINE, 0);
  lv_obj_set_style_line_width(line, 1, 0);
  return line;
}

lv_obj_t* make_vline(lv_obj_t* parent, int x, int y1, int y2) {
  lv_obj_t* line = lv_line_create(parent);
  static lv_point_t pts[2];
  pts[0] = {0, (lv_coord_t)y1};
  pts[1] = {0, (lv_coord_t)y2};
  lv_line_set_points(line, pts, 2);
  lv_obj_set_pos(line, x, 0);
  lv_obj_set_style_line_color(line, CLR_LINE, 0);
  lv_obj_set_style_line_width(line, 1, 0);
  return line;
}

// ===== BUILD CLOCK SCREEN =====
void ui_clock_build() {
  scr_clock = lv_obj_create(NULL);
  lv_obj_add_style(scr_clock, &style_bg, 0);

  // ── TOP BAR bg ──
  lv_obj_t* topbar = lv_obj_create(scr_clock);
  lv_obj_set_size(topbar, 320, 22);
  lv_obj_set_pos(topbar, 0, 0);
  lv_obj_set_style_bg_color(topbar, lv_color_make(8,12,20), 0);
  lv_obj_set_style_border_width(topbar, 0, 0);
  lv_obj_set_style_radius(topbar, 0, 0);
  lv_obj_set_style_pad_all(topbar, 0, 0);

  // Weather label (topbar kiri)
  lbl_weather = lv_label_create(topbar);
  lv_obj_add_style(lbl_weather, &style_small, 0);
  lv_obj_set_pos(lbl_weather, 4, 5);
  lv_label_set_text(lbl_weather, "* CERAH");

  // Alarm badge (topbar kanan)
  lbl_alarm = lv_label_create(topbar);
  lv_obj_set_style_text_color(lbl_alarm, CLR_RED, 0);
  lv_obj_set_style_text_font(lbl_alarm, &lv_font_montserrat_12, 0);
  lv_obj_align(lbl_alarm, LV_ALIGN_RIGHT_MID, -4, 0);
  lv_label_set_text(lbl_alarm, "AL 07:00");

  // Brightness bar (topbar tengah)
  bar_bright = lv_bar_create(topbar);
  lv_obj_set_size(bar_bright, 30, 5);
  lv_obj_set_pos(bar_bright, 150, 8);
  lv_obj_set_style_bg_color(bar_bright, lv_color_make(10,25,40), LV_PART_MAIN);
  lv_obj_set_style_bg_color(bar_bright, CLR_ACCENT, LV_PART_INDICATOR);
  lv_bar_set_range(bar_bright, 0, 100);
  lv_bar_set_value(bar_bright, 78, LV_ANIM_OFF);

  // Battery bar
  bar_bat = lv_bar_create(topbar);
  lv_obj_set_size(bar_bat, 20, 7);
  lv_obj_set_pos(bar_bat, 188, 7);
  lv_obj_set_style_bg_color(bar_bat, lv_color_make(6,12,10), LV_PART_MAIN);
  lv_obj_set_style_bg_color(bar_bat, CLR_GREEN, LV_PART_INDICATOR);
  lv_obj_set_style_border_color(bar_bat, lv_color_make(42,90,70), LV_PART_MAIN);
  lv_obj_set_style_border_width(bar_bat, 1, LV_PART_MAIN);
  lv_bar_set_range(bar_bat, 0, 100);
  lv_bar_set_value(bar_bat, 72, LV_ANIM_OFF);

  lbl_bat_pct = lv_label_create(topbar);
  lv_obj_set_style_text_font(lbl_bat_pct, &lv_font_montserrat_8, 0);
  lv_obj_set_style_text_color(lbl_bat_pct, lv_color_make(42,106,74), 0);
  lv_obj_set_pos(lbl_bat_pct, 212, 7);
  lv_label_set_text(lbl_bat_pct, "72%");

  // Topbar bottom line
  make_hline(scr_clock, 22, 0, 320);

  // ── JAM (center di area kiri 256px, tinggi 160px) ──
  // Area jam: x=0..255, y=22..181

  lbl_hour = lv_label_create(scr_clock);
  lv_obj_add_style(lbl_hour, &style_time, 0);
  lv_obj_set_pos(lbl_hour, 10, 55);
  lv_label_set_text(lbl_hour, "20");

  lbl_colon = lv_label_create(scr_clock);
  lv_obj_add_style(lbl_colon, &style_colon, 0);
  lv_obj_set_pos(lbl_colon, 110, 48);
  lv_label_set_text(lbl_colon, ":");

  lbl_min = lv_label_create(scr_clock);
  lv_obj_add_style(lbl_min, &style_time, 0);
  lv_obj_set_pos(lbl_min, 135, 55);
  lv_label_set_text(lbl_min, "27");

  // Divider vertikal kiri-kanan
  make_vline(scr_clock, 255, 22, 181);

  // ── RIGHT PANEL (x=256..319, y=22..181) ──
  // Suhu
  lbl_temp = lv_label_create(scr_clock);
  lv_obj_add_style(lbl_temp, &style_accent, 0);
  lv_obj_set_pos(lbl_temp, 258, 32);
  lv_label_set_text(lbl_temp, "25.3");

  lv_obj_t* lbl_unit = lv_label_create(scr_clock);
  lv_obj_add_style(lbl_unit, &style_dim, 0);
  lv_obj_set_pos(lbl_unit, 258, 62);
  lv_label_set_text(lbl_unit, "o C");

  // Divider tengah panel kanan
  make_hline(scr_clock, 80, 258, 318);

  // Bulan
  lbl_month = lv_label_create(scr_clock);
  lv_obj_add_style(lbl_month, &style_small, 0);
  lv_obj_set_pos(lbl_month, 270, 86);
  lv_label_set_text(lbl_month, "MAR");

  // Tanggal gede
  lbl_day = lv_label_create(scr_clock);
  lv_obj_set_style_text_color(lbl_day, CLR_ACCENT, 0);
  lv_obj_set_style_text_font(lbl_day, &lv_font_montserrat_36, 0);
  lv_obj_set_pos(lbl_day, 262, 100);
  lv_label_set_text(lbl_day, "19");

  // Hari
  lbl_dow = lv_label_create(scr_clock);
  lv_obj_add_style(lbl_dow, &style_dim, 0);
  lv_obj_set_pos(lbl_dow, 268, 148);
  lv_label_set_text(lbl_dow, "KAM");

  // ── FORECAST ROW (y=182..239) ──
  make_hline(scr_clock, 181, 0, 320);

  const char* fdays[]  = {"JUM","SAB","MIN","SEN"};
  const char* ftemps[] = {"28`","26`","24`","22`"};
  int fw = 80; // 320 / 4

  for (int i = 0; i < 4; i++) {
    int x = i * fw;
    if (i > 0) make_vline(scr_clock, x, 182, 239);

    lbl_fday[i] = lv_label_create(scr_clock);
    lv_obj_set_style_text_font(lbl_fday[i], &lv_font_montserrat_8, 0);
    lv_obj_set_style_text_color(lbl_fday[i], CLR_DIM, 0);
    lv_obj_set_pos(lbl_fday[i], x + fw/2 - 8, 188);
    lv_label_set_text(lbl_fday[i], fdays[i]);

    lbl_ftemp[i] = lv_label_create(scr_clock);
    lv_obj_set_style_text_font(lbl_ftemp[i], &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_ftemp[i], CLR_ACCENT2, 0);
    lv_obj_set_pos(lbl_ftemp[i], x + fw/2 - 10, 215);
    lv_label_set_text(lbl_ftemp[i], ftemps[i]);
  }
}

// ===== BUILD MENU SCREEN =====
void ui_menu_build() {
  scr_menu = lv_obj_create(NULL);
  lv_obj_add_style(scr_menu, &style_bg, 0);

  // Title
  lv_obj_t* title = lv_label_create(scr_menu);
  lv_obj_set_style_text_color(title, CLR_ACCENT, 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 8);
  lv_label_set_text(title, "== PENGATURAN ==");

  make_hline(scr_menu, 28, 10, 310);

  // Menu items
  for (int i = 0; i < MENU_COUNT; i++) {
    menu_items[i] = lv_obj_create(scr_menu);
    lv_obj_set_size(menu_items[i], 290, 32);
    lv_obj_set_pos(menu_items[i], 15, 34 + i * 36);
    lv_obj_add_style(menu_items[i], &style_menu_item, 0);

    lv_obj_t* lbl = lv_label_create(menu_items[i]);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(lbl, CLR_DIM, 0);
    lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 4, 0);
    lv_label_set_text(lbl, menu_labels[i]);

    menu_vals[i] = lv_label_create(menu_items[i]);
    lv_obj_set_style_text_font(menu_vals[i], &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(menu_vals[i], CLR_GREEN, 0);
    lv_obj_align(menu_vals[i], LV_ALIGN_RIGHT_MID, -4, 0);
  }

  lv_label_set_text(menu_vals[0], "78%");
  lv_label_set_text(menu_vals[1], "60s");
  lv_label_set_text(menu_vals[2], "07:00");
  lv_label_set_text(menu_vals[3], "24H");
  lv_label_set_text(menu_vals[4], "OK");

  // Hint bawah
  lv_obj_t* hint = lv_label_create(scr_menu);
  lv_obj_set_style_text_font(hint, &lv_font_montserrat_8, 0);
  lv_obj_set_style_text_color(hint, CLR_LINE, 0);
  lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -4);
  lv_label_set_text(hint, "UP/DN=PILIH  SEL=UBAH  MENU=KELUAR");
}

// ===== BUILD SLEEP SCREEN =====
void ui_sleep_build() {
  scr_sleep = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(scr_sleep, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(scr_sleep, LV_OPA_COVER, 0);
}

// ===== UPDATE FUNCTIONS =====
void ui_update_time(int h, int m, bool colon_visible) {
  if (!lbl_hour) return;
  char buf[4];
  sprintf(buf, "%02d", h); lv_label_set_text(lbl_hour, buf);
  sprintf(buf, "%02d", m); lv_label_set_text(lbl_min, buf);
  lv_obj_set_style_text_color(lbl_colon,
    colon_visible ? CLR_ACCENT : CLR_BG, 0);
}

void ui_update_date(int day, int month, int dow) {
  const char* months[] = {"JAN","FEB","MAR","APR","MEI","JUN",
                           "JUL","AGU","SEP","OKT","NOV","DES"};
  const char* dows[]   = {"MIN","SEN","SEL","RAB","KAM","JUM","SAB"};
  if (!lbl_day) return;
  char buf[4];
  sprintf(buf, "%02d", day);
  lv_label_set_text(lbl_day,   buf);
  lv_label_set_text(lbl_month, months[month-1]);
  lv_label_set_text(lbl_dow,   dows[dow]);
}

void ui_update_temp(float temp) {
  if (!lbl_temp) return;
  char buf[8]; sprintf(buf, "%.1f", temp);
  lv_label_set_text(lbl_temp, buf);
}

void ui_update_weather(const char* label) {
  if (!lbl_weather) return;
  char buf[20]; snprintf(buf, 20, "* %s", label);
  lv_label_set_text(lbl_weather, buf);
}

void ui_update_alarm(bool on, int h, int m) {
  if (!lbl_alarm) return;
  if (on) {
    char buf[12]; sprintf(buf, "AL %02d:%02d", h, m);
    lv_label_set_text(lbl_alarm, buf);
    lv_obj_set_style_text_color(lbl_alarm, CLR_RED, 0);
  } else {
    lv_label_set_text(lbl_alarm, "");
  }
}

void ui_update_brightness(int pct) {
  if (!bar_bright) return;
  lv_bar_set_value(bar_bright, pct, LV_ANIM_OFF);
}

void ui_update_battery(int pct) {
  if (!bar_bat) return;
  lv_bar_set_value(bar_bat, pct, LV_ANIM_OFF);
  char buf[6]; sprintf(buf, "%d%%", pct);
  lv_label_set_text(lbl_bat_pct, buf);
  lv_color_t c = (pct > 50) ? CLR_GREEN : (pct > 20) ? CLR_YELLOW : CLR_RED;
  lv_obj_set_style_bg_color(bar_bat, c, LV_PART_INDICATOR);
}

// ===== MENU NAVIGATION =====
void ui_menu_select(int idx) {
  menu_idx = idx;
  for (int i = 0; i < MENU_COUNT; i++) {
    lv_obj_remove_style(menu_items[i], &style_menu_sel, 0);
    lv_obj_add_style(menu_items[i], &style_menu_item, 0);
  }
  lv_obj_remove_style(menu_items[idx], &style_menu_item, 0);
  lv_obj_add_style(menu_items[idx], &style_menu_sel, 0);
}

void ui_menu_up()   { ui_menu_select((menu_idx - 1 + MENU_COUNT) % MENU_COUNT); }
void ui_menu_down() { ui_menu_select((menu_idx + 1) % MENU_COUNT); }
int  ui_menu_idx()  { return menu_idx; }

void ui_menu_set_val(int idx, const char* val) {
  if (idx < MENU_COUNT) lv_label_set_text(menu_vals[idx], val);
}

// ===== SCREEN SWITCH =====
void ui_show_clock() { lv_scr_load(scr_clock); }
void ui_show_menu()  { ui_menu_select(0); lv_scr_load(scr_menu); }
void ui_show_sleep() { lv_scr_load(scr_sleep); }

// ===== INIT =====
void ui_init() {
  ui_styles_init();
  ui_clock_build();
  ui_menu_build();
  ui_sleep_build();
  ui_show_clock();
}
