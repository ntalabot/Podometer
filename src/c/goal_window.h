#ifndef __GOAL_WINDOW_H__
#define __GOAL_WINDOW_H__

#include <pebble.h>

#define NUM_CELLS 1    // -> aussi changer dans layer.h le nombre max de cells
#define MAX_GOAL_VALUE 32000

typedef struct {
  int digits[NUM_CELLS];
} GOALS;

typedef void (*GoalWindowComplete)(GOALS goals, void *context);

typedef struct GoalWindowCallbacks {
    GoalWindowComplete goal_complete;
} GoalWindowCallbacks;

typedef struct {
  Window *window;
  TextLayer *main_text, *sub_text;
  Layer *selection;
  GColor highlight_color;
#ifdef PBL_SDK_3
  StatusBarLayer *status;
#endif
 GoalWindowCallbacks callbacks;

  GOALS goals;
  char field_buffs[NUM_CELLS][7];
  int8_t field_selection;
} GoalWindow;

/*
 * Creates a new PinWindow in memory but does not push it into view
 *  goal_window_callbacks: callbacks for communication
 *  returns: a pointer to a new PinWindow structure
 */
GoalWindow* goal_window_create(GoalWindowCallbacks goal_window_callbacks);

/*
 * Destroys an existing PinWindow
 *  goal_window: a pointer to the GoalWindow being destroyed
 */
void goal_window_destroy(GoalWindow *goal_window);

/*
 * Push the window onto the stack
 *  goal_window: a pointer to the GoalWindow being pushed
 *  animated: whether to animate the push or not
 */
void goal_window_push(GoalWindow *goal_window, bool animated);

/*
 * Pop the window off the stack
 *  goal_window: a pointer to the GoalWindow to pop
 *  animated: whether to animate the pop or not
 */
void goal_window_pop(GoalWindow *goal_window, bool animated);

/*
 * Gets whether it is the topmost window or not
 *  goal_window: a pointer to the GoalWindow being checked
 *  returns: a boolean indicating if it is the topmost window
 */
bool goal_window_get_topmost_window(GoalWindow *goal_window);

/*
 * Sets the over-all color scheme of the window
 *  color: the GColor to set the highlight to
 */
void goal_window_set_highlight_color(GoalWindow *goal_window, GColor color);

#endif