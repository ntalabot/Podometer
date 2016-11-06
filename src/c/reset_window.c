#include <pebble.h>
#include "reset_window.h"

static Window *s_main_window;
static TextLayer *s_label_layer;
static BitmapLayer *s_icon_layer;
static ActionBarLayer *s_action_bar_layer;

static GBitmap *s_tick_bitmap, *s_cross_bitmap;

extern int steps;

// Up click configuration
static void up_click_handler(ClickRecognizerRef recognizer, void *context)
{
	steps = 0;
	APP_LOG(APP_LOG_LEVEL_INFO, "Steps = %d", steps);
	window_stack_remove(s_main_window, true);
}

// Down click configuration
static void down_click_handler(ClickRecognizerRef recognizer, void *context)
{  window_stack_remove(s_main_window, true);  }


static void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	// Create the text layer
	const GEdgeInsets label_insets = { .top = 25,.right = ACTION_BAR_WIDTH,.left = ACTION_BAR_WIDTH / 10 };
	s_label_layer = text_layer_create(grect_inset(bounds, label_insets));
	text_layer_set_text(s_label_layer, "Do you really want to reset your steps ?");
	text_layer_set_background_color(s_label_layer, GColorClear);
	text_layer_set_text_alignment(s_label_layer, GTextAlignmentCenter);
	text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(s_label_layer));

	// Load icons Bitmaps
	s_tick_bitmap = gbitmap_create_with_resource(RESOURCE_ID_TICK);
	s_cross_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CROSS);

	// Create the ActionBarLayer
	s_action_bar_layer = action_bar_layer_create();
	action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_UP, s_tick_bitmap);
	action_bar_layer_set_icon(s_action_bar_layer, BUTTON_ID_DOWN, s_cross_bitmap);
	action_bar_layer_add_to_window(s_action_bar_layer, window);

	action_bar_layer_set_click_config_provider(s_action_bar_layer, click_config_provider);
}

static void window_unload(Window *window)
{
	text_layer_destroy(s_label_layer);
	action_bar_layer_destroy(s_action_bar_layer);
	bitmap_layer_destroy(s_icon_layer);

	gbitmap_destroy(s_tick_bitmap);
	gbitmap_destroy(s_cross_bitmap);

	window_destroy(window);
	s_main_window = NULL;
}

//   Push the window onto the stack
void reset_window_push()
{
	if (!s_main_window) {
		s_main_window = window_create();
		window_set_background_color(s_main_window, PBL_IF_COLOR_ELSE(GColorJaegerGreen, GColorWhite));
		window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load, 
      .unload = window_unload,});
	}
	window_stack_push(s_main_window, true);
}
