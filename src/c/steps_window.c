#include <pebble.h>
#include "steps_window.h"
#include "distance_window.h"
#include "menu_window.h"

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GFont s_res_gothic_28_bold;
static GFont s_res_gothic_24_bold;
static Layer *s_window_layer;
static Layer *s_progress_layer;
//static TextLayer *s_textlayer_1;
static TextLayer *s_textlayer_2;
static TextLayer *s_textlayer_3;
//static TextLayer *s_textlayer_4;
StatusBarLayer *status_bar_layer;

// Variables
//static uint16_t steps = 6283;
//static uint16_t goal = 10000;
extern int steps;
extern int goal;
static char buffer1[16];
static char buffer2[16];
static char testbuffer[25];
//static char s_emoji[5];

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //hide_steps_window();
  show_menu_window();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  hide_steps_window();
  show_distance_window();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void display_value(uint16_t value, char buffer[]) {
  int thousands = value / 1000;
  int hundreds = value % 1000;
  //static char s_emoji[5];
  
  //snprintf(s_emoji, sizeof(s_emoji), "\U0001F60C");

  if(thousands > 0) {
    //snprintf(buffer1, sizeof(buffer1), "%d'%03d %s", thousands, hundreds, s_emoji);
    snprintf(buffer, sizeof(buffer1), "%d'%03d", thousands, hundreds);
  } else {
    //snprintf(buffer1, sizeof(buffer1), "%d %s", hundreds, s_emoji);
    snprintf(buffer, sizeof(buffer1), "%d", hundreds);
  }

  //text_layer_set_text(s_textlayer_2, buffer1);
}

static void progress_layer_update_proc(Layer *layer, GContext *ctx) {
  APP_LOG(APP_LOG_LEVEL_INFO, "progress update");
  GRect inset;
  
  if(steps > goal) {
    // progress bar Gray
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
  graphics_context_set_fill_color(ctx,GColorDarkGray);
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 8, DEG_TO_TRIGANGLE(0),
  //DEG_TO_TRIGANGLE(360 * ((float)(steps) / (float)(goal))));
  DEG_TO_TRIGANGLE(360));
    
    // progress bar White
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
  graphics_context_set_fill_color(ctx,GColorWhite);
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 8, DEG_TO_TRIGANGLE(0),
  //DEG_TO_TRIGANGLE(360 * ((float)(steps) / (float)(goal))));
  DEG_TO_TRIGANGLE(360 * (steps-goal) / goal));
  
  // external circle
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
  graphics_context_set_fill_color(ctx,GColorWhite);
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 1, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
  
  // internal circle
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(24));
  graphics_context_set_fill_color(ctx,GColorWhite);
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 1, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));

  // start marker
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(14));
  graphics_context_set_fill_color(ctx, GColorWhite);
  int trigangle = DEG_TO_TRIGANGLE(0);
  int line_width_trigangle = 560;
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 14, trigangle - line_width_trigangle, trigangle);
  
  // end marker
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(17));
  graphics_context_set_fill_color(ctx, GColorBlack);
  trigangle = DEG_TO_TRIGANGLE(360 * (steps-goal) / goal);
  line_width_trigangle = 400;
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 7, trigangle - line_width_trigangle, trigangle);
    
  } else {
    // progress bar
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
  graphics_context_set_fill_color(ctx,GColorDarkGray);
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 8, DEG_TO_TRIGANGLE(0),
  //DEG_TO_TRIGANGLE(360 * ((float)(steps) / (float)(goal))));
  DEG_TO_TRIGANGLE(360 * steps / goal));
  
  // external circle
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
  graphics_context_set_fill_color(ctx,GColorWhite);
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 1, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
  
  // internal circle
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(24));
  graphics_context_set_fill_color(ctx,GColorWhite);
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 1, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));

  // start marker
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(14));
  graphics_context_set_fill_color(ctx, GColorWhite);
  int trigangle = DEG_TO_TRIGANGLE(0);
  int line_width_trigangle = 560;
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 14, trigangle - line_width_trigangle, trigangle);
  
  // end marker
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
  graphics_context_set_fill_color(ctx, GColorWhite);
  trigangle = DEG_TO_TRIGANGLE(360 * steps / goal);
  line_width_trigangle = 400;
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 8, trigangle - line_width_trigangle, trigangle);
  }
  
}

static void initialise_ui(void) {
  s_window = window_create();
  s_window_layer = window_get_root_layer(s_window);
  //GRect window_bounds = layer_get_bounds(s_window_layer);
  window_set_click_config_provider(s_window, click_config_provider);    // buttons interactions
  window_set_background_color(s_window, GColorBlack);
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, false);
  #endif
  
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  
      // status bar
  status_bar_layer = status_bar_layer_create();
  status_bar_layer_set_colors(status_bar_layer, GColorBlack, GColorWhite);
  status_bar_layer_set_separator_mode(status_bar_layer, StatusBarLayerSeparatorModeDotted);
  layer_add_child(s_window_layer, (Layer *)status_bar_layer);
  
      // progress bar
  s_progress_layer = layer_create(GRect(0, 7, 144, 150));
  layer_set_update_proc(s_progress_layer, progress_layer_update_proc);
  layer_add_child(s_window_layer, (Layer *)s_progress_layer);
  
      // s_textlayer_1
  //s_textlayer_1 = text_layer_create(GRect(0, 15, 144, 25));
  //text_layer_set_background_color(s_textlayer_1, GColorBlack);
  //text_layer_set_text_color(s_textlayer_1, GColorWhite);
  //text_layer_set_text(s_textlayer_1, "Steps");
  //text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  //text_layer_set_font(s_textlayer_1, s_res_gothic_24_bold);
  //layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
  
      // steps counter
  s_textlayer_2 = text_layer_create(GRect(41, 48, 62, 30));
  text_layer_set_background_color(s_textlayer_2, GColorBlack);
  text_layer_set_text_color(s_textlayer_2, GColorWhite);
  
  display_value(steps, buffer1);
  //snprintf(buffer1, sizeof(buffer1), "%d", steps);    // prints value of steps in buffer
  text_layer_set_text(s_textlayer_2, buffer1);
  
  text_layer_set_font(s_textlayer_2, s_res_gothic_28_bold);
  text_layer_set_text_alignment(s_textlayer_2, GTextAlignmentCenter);
  
  //layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);
  layer_add_child(s_window_layer, (Layer *)s_textlayer_2);
  
      // goal
  s_textlayer_3 = text_layer_create(GRect(0, 143, 144, 20));
  text_layer_set_background_color(s_textlayer_3, GColorBlack);
  text_layer_set_text_color(s_textlayer_3, GColorWhite);
  
  display_value(goal, buffer2);
  //snprintf(buffer2, sizeof(buffer2), "%d", goal);
  snprintf(testbuffer, sizeof(testbuffer),"%s %s", "Current goal:", buffer2);
  text_layer_set_text(s_textlayer_3, testbuffer);
  text_layer_set_text_alignment(s_textlayer_3, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_3, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_3);
  
      // s_textlayer_4
  //s_textlayer_4 = text_layer_create(GRect(0, 115, 144, 30));
  //text_layer_set_background_color(s_textlayer_4, GColorBlack);
  //text_layer_set_text_color(s_textlayer_4, GColorWhite);
  
  //snprintf(s_emoji, sizeof(s_emoji), "\U0001F60C");
  //text_layer_set_text(s_textlayer_4, s_emoji);
  
  //snprintf(buffer2, sizeof(buffer2), "%d", goal);    // prints value of steps in buffer
  //text_layer_set_text(s_textlayer_4, buffer2);
  //text_layer_set_text_alignment(s_textlayer_4, GTextAlignmentCenter);
  //text_layer_set_font(s_textlayer_4, s_res_gothic_28_bold);
  //layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_4);
  
}

static void destroy_ui(void) {
  window_destroy(s_window);
  status_bar_layer_destroy(status_bar_layer);
  layer_destroy(s_progress_layer);
  //text_layer_destroy(s_textlayer_1);
  text_layer_destroy(s_textlayer_2);
  text_layer_destroy(s_textlayer_3);
  //text_layer_destroy(s_textlayer_4);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_steps_window(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_steps_window(void) {
  window_stack_remove(s_window, true);
}
