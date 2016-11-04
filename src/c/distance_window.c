#include <pebble.h>
#include "distance_window.h"
#include "steps_window.h"
#include "menu_window.h"

static Window *s_window;
static Layer *s_progress_layer;
static GFont s_res_gothic_28_bold;
static GFont s_res_gothic_24_bold;
static TextLayer *s_textlayer_2;
static TextLayer *s_textlayer_4;
StatusBarLayer *status_bar_layer;

// Variables
extern int steps;
extern int goal;
extern int size;
extern float gender;

static float distance;
static float d_goal;
static char buffer1[16];
static char buffer2[16];
static char testbuffer[30];
static char testbuffer2[30];

// source: https://forums.pebble.com/t/displaying-the-value-of-a-floating-point/6080/6
char* floatToString(char* buffer, int bufferSize, float number)
{
char decimalBuffer[5];

snprintf(buffer, bufferSize, "%d", (int)number);
strcat(buffer, ".");

snprintf(decimalBuffer, 5, "%02d", (int)((float)(number - (int)number) * (float)100));
strcat(buffer, decimalBuffer);

return buffer;
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //hide_distance_window();
  show_menu_window();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  hide_distance_window();
  show_steps_window();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
}

static void progress_layer_update_proc(Layer *layer, GContext *ctx) {
  APP_LOG(APP_LOG_LEVEL_INFO, "progress update");
  GRect inset;
  
  if(steps > goal) {
    // progress bar Gray
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
  graphics_context_set_fill_color(ctx,GColorDarkGray);
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 8, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));
    
    // progress bar White
  inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
  graphics_context_set_fill_color(ctx,GColorWhite);
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 8, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360 * (steps-goal) / goal));
  
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
  graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 8, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360 * steps / goal));
  
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
  
  // Update text_layers
  distance = (steps * ((gender * size)/100))/1000;
  floatToString(buffer1, sizeof(buffer1), distance);
  snprintf(testbuffer, sizeof(testbuffer),"%s %s", buffer1, "km");
  text_layer_set_text(s_textlayer_2, testbuffer);
  
  d_goal = (goal * ((gender * size)/100))/1000;
  floatToString(buffer2, sizeof(buffer2), d_goal);
  snprintf(testbuffer2, sizeof(testbuffer2),"%s %s %s", "Current goal:", buffer2, "km");
  text_layer_set_text(s_textlayer_4, testbuffer2);
}

static void initialise_ui(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, click_config_provider);    // buttons interactions
  window_set_background_color(s_window, GColorBlack);
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, false);
  #endif
  
  s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  
  // Create status bar Layer
  status_bar_layer = status_bar_layer_create();
  status_bar_layer_set_colors(status_bar_layer, GColorBlack, GColorWhite);
  status_bar_layer_set_separator_mode(status_bar_layer, StatusBarLayerSeparatorModeDotted);
  layer_add_child(window_get_root_layer(s_window), (Layer *)status_bar_layer);
  
  // progress bar
  s_progress_layer = layer_create(GRect(0, 7, 144, 150));
  layer_set_update_proc(s_progress_layer, progress_layer_update_proc);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_progress_layer);
  
  // s_textlayer_2
  s_textlayer_2 = text_layer_create(GRect(32, 60, 80, 30));
  text_layer_set_background_color(s_textlayer_2, GColorBlack);
  text_layer_set_text_color(s_textlayer_2, GColorWhite);
  //distance = (steps * ((gender * size)/100))/1000;
  //floatToString(buffer1, sizeof(buffer1), distance);
  //snprintf(testbuffer, sizeof(testbuffer),"%s %s", buffer1, "km");
  //text_layer_set_text(s_textlayer_2, testbuffer);
  text_layer_set_text_alignment(s_textlayer_2, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_2, s_res_gothic_28_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);
  
  // s_textlayer_4
  s_textlayer_4 = text_layer_create(GRect(0, 143, 144, 20));
  text_layer_set_background_color(s_textlayer_4, GColorBlack);
  text_layer_set_text_color(s_textlayer_4, GColorWhite);
  //d_goal = (goal * ((gender * size)/100))/1000;
  //floatToString(buffer2, sizeof(buffer2), d_goal);
  //snprintf(testbuffer2, sizeof(testbuffer2),"%s %s %s", "Current goal:", buffer2, "km");
  //text_layer_set_text(s_textlayer_4, testbuffer2);
  text_layer_set_font(s_textlayer_4, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_textlayer_4, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_4);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_2);
  text_layer_destroy(s_textlayer_4);
  status_bar_layer_destroy(status_bar_layer);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_distance_window(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_distance_window(void) {
  window_stack_remove(s_window, true);
}
