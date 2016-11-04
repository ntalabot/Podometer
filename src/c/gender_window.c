#include <pebble.h>
#include "gender_window.h"

extern float gender;

static Window *s_main_window;
static MenuLayer *s_menu_layer;

static int s_current_selection = 0;

// Give the total number of rows
static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return GENDER_WINDOW_NUM_ROWS;
}

// Draw the layout
static void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) 
{
	switch(cell_index->row)
	{
	case 0:
		menu_cell_basic_draw(ctx, cell_layer, "Female",NULL, NULL);
		break;
	case 1:
		menu_cell_basic_draw(ctx, cell_layer, "Male",NULL, NULL);
		break;
  }
    
    // Create the cell button
  GRect bounds = layer_get_bounds(cell_layer);
  GPoint p = GPoint(bounds.size.w - (3 * GENDER_WINDOW_BUTTON_RADIUS), (bounds.size.h / 2));

    // Black highlighted if selected
    if(menu_cell_layer_is_highlighted(cell_layer)) 
    {
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    } 
    else
     { graphics_context_set_fill_color(ctx, GColorBlack);}

    // Draw cell button filled/empty
    graphics_draw_circle(ctx, p, GENDER_WINDOW_BUTTON_RADIUS);
    if(cell_index->row == s_current_selection) // selected cell button
    { graphics_fill_circle(ctx, p, GENDER_WINDOW_BUTTON_RADIUS - 3); }

}

static int16_t get_cell_height_callback(struct MenuLayer *s_menu_layer, MenuIndex *cell_index, void *context) 
{
  return PBL_IF_ROUND_ELSE(menu_layer_is_index_selected(s_menu_layer, cell_index) ? 
         MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT, 44);
}

// Decide what happens when the selected button is pressed
static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  if(cell_index->row == GENDER_WINDOW_NUM_ROWS ) 
  {  window_stack_pop(true);  } 
  else  // Change selection
  { 
    s_current_selection = cell_index->row;
    menu_layer_reload_data(menu_layer);
  }
  
  if (s_current_selection == 0)  // Female selection
  {  gender = FEMALE;
     APP_LOG(APP_LOG_LEVEL_INFO, "female");
  }
  else
  {  gender = MALE;
     APP_LOG(APP_LOG_LEVEL_INFO, "male");
  }
  window_stack_remove(s_main_window, true);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback,
      .draw_row = draw_row_callback,
      .get_cell_height = get_cell_height_callback,
      .select_click = select_callback,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);

  window_destroy(window);
  s_main_window = NULL;
}

void gender_window_push() {
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}