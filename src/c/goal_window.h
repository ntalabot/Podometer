#ifndef __GOAL_WINDOW_H__
#define __GOAL_WINDOW_H__

#include <pebble.h>

/* THIS FILE IS GREATLY INSPIRED OF THE CODE IN THE FOLLOWING LINK
http://www.mediafire.com/file/btramcjbtnq1a9w/pinentrytestmodification.zip  */

#define NUM_CELLS 1            
#define MAX_GOAL_VALUE 50000

typedef struct {
	int digits[NUM_CELLS];
} GOALS;

typedef void(*GoalWindowComplete)(GOALS goals, void *context);

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

// Creates the GoalWindow in memory but does not push it into view
GoalWindow* goal_window_create(GoalWindowCallbacks goal_window_callbacks);

// Destroys the GoalWindow
void goal_window_destroy(GoalWindow *goal_window);

// Push the window onto the stack
void goal_window_push(GoalWindow *goal_window, bool animated);

// Pop the window off the stack
void goal_window_pop(GoalWindow *goal_window, bool animated);

// Gets whether it is the topmost window or not
bool goal_window_get_topmost_window(GoalWindow *goal_window);

// Sets the over-all color scheme of the window
void goal_window_set_highlight_color(GoalWindow *goal_window, GColor color);

#endif