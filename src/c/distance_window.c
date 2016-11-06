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

// Global variables for the person's infos, declared in main.c
extern int steps;
extern int goal;
extern int size;
extern float gender;

// Local variables for distance infos and text displaying
static float distance;
static float d_goal;
static char buffer1[16];
static char buffer2[16];
static char finalbuffer[30];
static char finalbuffer2[30];

// Print a float value in a char buffer
// source: https://forums.pebble.com/t/displaying-the-value-of-a-floating-point/6080/6
char* floatToString(char* buffer, int bufferSize, float number) {
  char decimalBuffer[5];

	snprintf(buffer, bufferSize, "%d", (int)number);
	strcat(buffer, ".");

	snprintf(decimalBuffer, 5, "%02d", (int)((float)(number - (int)number) * (float)100));
	strcat(buffer, decimalBuffer);

	return buffer;
}

// Select button shows the menu
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	show_menu_window();
}

// Up button changes the window to steps
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	hide_distance_window();
	show_steps_window();
}

// Assigned buttons to their associated functions
static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
}

// Update the display of the progress circle and distance
static void progress_layer_update_proc(Layer *layer, GContext *ctx) {
	GRect inset;

  // If the current goal is overpassed, the progress bar continues with another color
	if (steps > goal) {
		// progress bar Gray
		inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
		graphics_context_set_fill_color(ctx, GColorDarkGray);
		graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 8, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));

		// progress bar White
		inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 8, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360 * (steps - goal) / goal));

		// external circle
		inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 1, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));

		// internal circle
		inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(24));
		graphics_context_set_fill_color(ctx, GColorWhite);
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
		trigangle = DEG_TO_TRIGANGLE(360 * (steps - goal) / goal);
		line_width_trigangle = 400;
		graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 7, trigangle - line_width_trigangle, trigangle);

	}
	else { // Or display the simple progress bar toward goal
		// progress bar
		inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
		graphics_context_set_fill_color(ctx, GColorDarkGray);
		graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 8, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360 * steps / goal));

		// external circle
		inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(16));
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_radial(ctx, inset, GOvalScaleModeFitCircle, 1, DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360));

		// internal circle
		inset = grect_inset(layer_get_bounds(layer), GEdgeInsets(24));
		graphics_context_set_fill_color(ctx, GColorWhite);
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

	// Update the text layers (distance and goal)
	distance = (steps * ((gender * size) / 100)) / 1000;
	floatToString(buffer1, sizeof(buffer1), distance);
	snprintf(finalbuffer, sizeof(finalbuffer), "%s %s", buffer1, "km");
	text_layer_set_text(s_textlayer_2, finalbuffer);

	d_goal = (goal * ((gender * size) / 100)) / 1000;
	floatToString(buffer2, sizeof(buffer2), d_goal);
	snprintf(finalbuffer2, sizeof(finalbuffer2), "%s %s %s", "Current goal:", buffer2, "km");
	text_layer_set_text(s_textlayer_4, finalbuffer2);
}

// Init function called when the window is created
static void initialise_ui(void) {
	s_window = window_create();
	window_set_click_config_provider(s_window, click_config_provider);    // buttons interactions
	window_set_background_color(s_window, GColorBlack);
#ifndef PBL_SDK_3
	window_set_fullscreen(s_window, false);
#endif

	s_res_gothic_28_bold = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);

	// Create status bar Layer
	status_bar_layer = status_bar_layer_create();
	status_bar_layer_set_colors(status_bar_layer, GColorBlack, GColorWhite);
	status_bar_layer_set_separator_mode(status_bar_layer, StatusBarLayerSeparatorModeDotted);
	layer_add_child(window_get_root_layer(s_window), (Layer *)status_bar_layer);

	// distance
	s_textlayer_2 = text_layer_create(GRect(31, 62, 82, 30));
	text_layer_set_background_color(s_textlayer_2, GColorBlack);
	text_layer_set_text_color(s_textlayer_2, GColorWhite);
  text_layer_set_font(s_textlayer_2, s_res_gothic_28_bold);
	text_layer_set_text_alignment(s_textlayer_2, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_2);
  
  // progress bar
	s_progress_layer = layer_create(GRect(0, 7, 144, 150));
	layer_set_update_proc(s_progress_layer, progress_layer_update_proc);
	layer_add_child(window_get_root_layer(s_window), (Layer *)s_progress_layer);

	// current goal
	s_textlayer_4 = text_layer_create(GRect(0, 143, 144, 20));
	text_layer_set_background_color(s_textlayer_4, GColorBlack);
	text_layer_set_text_color(s_textlayer_4, GColorWhite);
	text_layer_set_font(s_textlayer_4, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(s_textlayer_4, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_4);
}

// Deinit function called when the window is destroyed
static void destroy_ui(void) {
	window_destroy(s_window);
	text_layer_destroy(s_textlayer_2);
	text_layer_destroy(s_textlayer_4);
	status_bar_layer_destroy(status_bar_layer);
}

static void handle_window_unload(Window* window) {
	destroy_ui();
}

// Display the distance window on the Pebble
void show_distance_window(void) {
	initialise_ui();
	window_set_window_handlers(s_window, (WindowHandlers) {
		.unload = handle_window_unload,
	});
	window_stack_push(s_window, true);
}

// Ask for an update of the distance window to display current values
void update_distance_window(void) {
	if (s_window)
		layer_mark_dirty(s_progress_layer);
}

// Hide the distance window, so that another window can be displayed
void hide_distance_window(void) {
	window_stack_remove(s_window, true);
}