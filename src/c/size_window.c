#include <pebble.h>
#include "size_window.h"
#include "layers_selection.h"

/* THIS FILE IS GREATLY INSPIRED OF THE CODE IN THE FOLLOWING LINK
http://www.mediafire.com/file/btramcjbtnq1a9w/pinentrytestmodification.zip  */

// Global variables for the person's infos, declared in main.c
extern int size;

static char* selection_handle_get_text(int index, void *context) {
	SizeWindow *size_window = (SizeWindow*)context;
	snprintf(
		size_window->field_buffs[index],
		sizeof(size_window->field_buffs[0]), "%d", (int)size_window->pin.digits[index]);
	return size_window->field_buffs[index];
}

static void selection_handle_complete(void *context) {
	SizeWindow *size_window = (SizeWindow*)context;
	size_window->callbacks.size_complete(size_window->pin, size_window);
}

// Size incementing
static void selection_handle_inc(int index, uint8_t clicks, void *context) {
	SizeWindow *size_window = (SizeWindow*)context;
	size_window->pin.digits[index]++;
	if (size_window->pin.digits[index] > MAX_SIZE)
	{
		size_window->pin.digits[index] = MIN_SIZE;
	}
}

// Size decrementing
static void selection_handle_dec(int index, uint8_t clicks, void *context) {
	SizeWindow *size_window = (SizeWindow*)context;
	size_window->pin.digits[index]--;
	if (size_window->pin.digits[index] < MIN_SIZE)
	{
		size_window->pin.digits[index] = MAX_SIZE;
	}
}

// Size window creation
SizeWindow* size_window_create(SizeWindowCallbacks callbacks) {
	SizeWindow *size_window = (SizeWindow*)malloc(sizeof(SizeWindow));
	if (size_window) {
		size_window->window = window_create();
		size_window->callbacks = callbacks;
		if (size_window->window) {
			size_window->field_selection = 0;
			size_window->pin.digits[0] = size;


			// Get window parameters
			Layer *window_layer = window_get_root_layer(size_window->window);
			GRect bounds = layer_get_bounds(window_layer);

			// Main TextLayer
			size_window->main_text = text_layer_create(GRect(0, 30, bounds.size.w, 40));
			text_layer_set_text(size_window->main_text, "Size");
			text_layer_set_font(size_window->main_text, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
			text_layer_set_text_alignment(size_window->main_text, GTextAlignmentCenter);
			layer_add_child(window_layer, text_layer_get_layer(size_window->main_text));

			// Sub TextLayer
			size_window->sub_text = text_layer_create(GRect(1, 125, bounds.size.w, 40));
			text_layer_set_text(size_window->sub_text, "cm");
			text_layer_set_text_alignment(size_window->sub_text, GTextAlignmentCenter);
			text_layer_set_font(size_window->sub_text, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
			layer_add_child(window_layer, text_layer_get_layer(size_window->sub_text));

			// Create selection layer
			size_window->selection = selection_layer_create(GRect(30, 75, 80, 30), NUM_CELLS);
			selection_layer_set_cell_width(size_window->selection, 0, 80); // taille largeur
			selection_layer_set_cell_padding(size_window->selection, 10);
			selection_layer_set_active_bg_color(size_window->selection, GColorDarkGray);
			selection_layer_set_click_config_onto_window(size_window->selection, size_window->window);
    
      // Set callbacks
			selection_layer_set_callbacks(size_window->selection, size_window, (SelectionLayerCallbacks)
			{
				.get_cell_text = selection_handle_get_text,
					.complete = selection_handle_complete,
					.increment = selection_handle_inc,
					.decrement = selection_handle_dec,
			});
      
      // Add to window
			layer_add_child(window_get_root_layer(size_window->window), size_window->selection);
			return size_window;
		}
	}

	APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to create SizeWindow");
	return NULL;
}

// Destroys the SizeWindow
void size_window_destroy(SizeWindow *size_window) {
	if (size_window)
	{
		selection_layer_destroy(size_window->selection);
		text_layer_destroy(size_window->sub_text);
		text_layer_destroy(size_window->main_text);
		free(size_window);
		size_window = NULL;
		return;
	}
}

// Push the window onto the stack
void size_window_push(SizeWindow *size_window, bool animated) { 
  window_stack_push(size_window->window, animated); 
}

// Push the window off the stack
void size_window_pop(SizeWindow *size_window, bool animated) {  
  window_stack_remove(size_window->window, animated);  
}

// Gets whether it is the topmost window or not
bool size_window_get_topmost_window(SizeWindow *size_window) {
  return window_stack_get_top_window() == size_window->window;  
}

// Sets the over-all color scheme of the window
void size_window_set_highlight_color(SizeWindow *size_window, GColor color) {
	size_window->highlight_color = color;
	selection_layer_set_active_bg_color(size_window->selection, color);
}
