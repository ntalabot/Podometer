#include <pebble.h>
#include "gender_window.h"
#include "size_window.h"
#include "goal_window.h"
#include "menu_window.h"
#include "welcome_window.h"
#include "reset_window.h"

#define NUM_WINDOWS_MENU 4    // Goal, Gender, Size and Reset rows

extern int size;
extern int goal;
extern float gender;

static Window *s_window;
static MenuLayer *menu_layer;
static TextLayer *s_text_layer;

// SIZE WINDOW Save the size and return to menu window
static void size_complete_callback(PIN pin, void *context) 
{
	size = pin.digits[0];
	APP_LOG(APP_LOG_LEVEL_INFO, "Size is %d", size);
	size_window_pop((SizeWindow*)context, true);
}

// GOAL WINDOW Save the goal and return to menu window
static void goal_complete_callback(GOALS goals, void *context) 
{
	goal = goals.digits[0];
	APP_LOG(APP_LOG_LEVEL_INFO, "Goal is %d", goal);
	goal_window_pop((GoalWindow*)context, true);
}

// Give the total number of rows
static uint16_t num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{  return NUM_WINDOWS_MENU;  }

// Create the rows
static void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
	switch (cell_index->row)
	{
	case 0:
		menu_cell_basic_draw(ctx, cell_layer, "Goal", NULL, NULL);
		break;
	case 1:
		menu_cell_basic_draw(ctx, cell_layer, "Gender", NULL, NULL);
		break;
	case 2:
		menu_cell_basic_draw(ctx, cell_layer, "Size", NULL, NULL);
		break;
	case 3:
		menu_cell_basic_draw(ctx, cell_layer, "Reset", NULL, NULL);
		break;
	}
}

// Callback function when a row is selected
void select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
	switch (cell_index->row)
	{
	  case 0:      //Open the goal window
	  { GoalWindow *goal_window = goal_window_create((GoalWindowCallbacks) { .goal_complete = goal_complete_callback	});
  		goal_window_push(goal_window, true);
	  	break; }
  	case 1:      //Open the gender window
    { gender_window_push();
     break;  }
  	case 2:      //Open the size window
  	{ SizeWindow *size_window = size_window_create((SizeWindowCallbacks) { .size_complete = size_complete_callback	});
		  size_window_push(size_window, true);
		  break;  }
	  case 3:      //Open the reset window
	  {	reset_window_push();
      break;  }
	}
}

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
//static Window *s_window;

//static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
//  hide_menu_window();
//  show_welcome_window();
//}

//static void click_config_provider(void *context) {
//  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
//}

// Create the layout 
static void initialise_ui(void) {
	s_window = window_create();
#ifndef PBL_SDK_3
	window_set_fullscreen(s_window, true);
#endif

	// Top text_layer
	s_text_layer = text_layer_create(GRect(0, 0, 144, 18));
	text_layer_set_background_color(s_text_layer, GColorBlack);
	text_layer_set_text_color(s_text_layer, GColorWhite);
	text_layer_set_text(s_text_layer, "Settings");
	text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(s_window), text_layer_get_layer(s_text_layer));

  // Rows layout
	menu_layer = menu_layer_create(GRect(0, 18, 144, 168));
	menu_layer_set_normal_colors(menu_layer, GColorBlack, GColorWhite);
	menu_layer_set_highlight_colors(menu_layer, GColorWhite, GColorBlack);

	//Let it receive clicks
	menu_layer_set_click_config_onto_window(menu_layer, s_window);
	//window_set_click_config_provider(s_window, click_config_provider);

	//Set the callbacks
	MenuLayerCallbacks callbacks = {
		.draw_row = (MenuLayerDrawRowCallback)draw_row_callback,
		.get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback)num_rows_callback,
		.select_click = (MenuLayerSelectCallback)select_click_callback
	};
	menu_layer_set_callbacks(menu_layer, NULL, callbacks);

	//Add to Window
	layer_add_child(window_get_root_layer(s_window), menu_layer_get_layer(menu_layer));
}

static void destroy_ui(void) {
	window_destroy(s_window);
	menu_layer_destroy(menu_layer);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
	destroy_ui();
}

void show_menu_window(void) {
	initialise_ui();
	window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
	window_stack_push(s_window, true);
}

void hide_menu_window(void) {
	window_stack_remove(s_window, true);
}
