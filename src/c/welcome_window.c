#include <pebble.h>
#include "welcome_window.h"

static Window *s_window;
static GFont s_res_gothic_14;
static GBitmap *s_res_image_man_icon;
static TextLayer *s_textlayer_1;
static TextLayer *s_textlayer_2;
static TextLayer *s_textlayer_3;
static TextLayer *s_textlayer_4;
static BitmapLayer *s_bitmaplayer_1;

// Init function called when the window is created
static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
  window_set_fullscreen(s_window, true);
  #endif
  
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_res_image_man_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MAN_ICON);
  
  // Name 2 
  s_textlayer_1 = text_layer_create(GRect(22, 105, 100, 20));
  text_layer_set_text(s_textlayer_1, "Quentin Golay");
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_1, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_1);
  
  // Name 4
  s_textlayer_2 = text_layer_create(GRect(20, 145, 100, 20));
  text_layer_set_text(s_textlayer_2, "Nicolas Talabot");
  text_layer_set_text_alignment(s_textlayer_2, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_2, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);
  
  // Name 1
  s_textlayer_3 = text_layer_create(GRect(20, 85, 100, 20));
  text_layer_set_text(s_textlayer_3, "Léa Bole-Feysot");
  text_layer_set_text_alignment(s_textlayer_3, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_3, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_3);
  
  // Name 3
  s_textlayer_4 = text_layer_create(GRect(20, 125, 100, 20));
  text_layer_set_text(s_textlayer_4, "Benjamin Ramtoula");
  text_layer_set_text_alignment(s_textlayer_4, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_4, s_res_gothic_14);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_4);
  
  // walking man image
  s_bitmaplayer_1 = bitmap_layer_create(GRect(37, 10, 70, 70));
  bitmap_layer_set_bitmap(s_bitmaplayer_1, s_res_image_man_icon);
  bitmap_layer_set_background_color(s_bitmaplayer_1, GColorWhite);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_bitmaplayer_1);
}

// Deinit function called when the window is destroyed
static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_1);
  text_layer_destroy(s_textlayer_2);
  text_layer_destroy(s_textlayer_3);
  text_layer_destroy(s_textlayer_4);
  bitmap_layer_destroy(s_bitmaplayer_1);
  gbitmap_destroy(s_res_image_man_icon);
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

// Display the welcome window on the Pebble
void show_welcome_window(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {.unload = handle_window_unload,});
  window_stack_push(s_window, true);
}

// Hide the welcome window, so that another window can be displayed
void hide_welcome_window(void) {
  window_stack_remove(s_window, true);
}
