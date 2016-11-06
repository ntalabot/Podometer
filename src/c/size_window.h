#ifndef __SIZE_WINDOW_H__
#define __SIZE_WINDOW_H__

#include <pebble.h>

/* THIS FILE IS GREATLY INSPIRED OF THE CODE IN THE FOLLOWING LINK
http://www.mediafire.com/file/btramcjbtnq1a9w/pinentrytestmodification.zip  */

#define NUM_CELLS 1   
#define MAX_SIZE 250
#define MIN_SIZE 70

typedef struct {
	int digits[NUM_CELLS];
} PIN;

typedef void(*SizeWindowComplete)(PIN pin, void *context);

typedef struct SizeWindowCallbacks {
	SizeWindowComplete size_complete;
} SizeWindowCallbacks;

typedef struct {
	Window *window;
	TextLayer *main_text, *sub_text;
	Layer *selection;
	GColor highlight_color;
#ifdef PBL_SDK_3
	StatusBarLayer *status;
#endif
	SizeWindowCallbacks callbacks;

	PIN pin;
	char field_buffs[NUM_CELLS][5];
	int8_t field_selection;
} SizeWindow;

// Creates the SizeWindow in memory but does not push it into view
SizeWindow* size_window_create(SizeWindowCallbacks size_window_callbacks);

// Destroys the SizeWindow
void size_window_destroy(SizeWindow *size_window);

// Push the window onto the stack
void size_window_push(SizeWindow *size_window, bool animated);

// Pop the window off the stack
void size_window_pop(SizeWindow *size_window, bool animated);

// Gets whether it is the topmost window or not
bool size_window_get_topmost_window(SizeWindow *size_window);

// Sets the over-all color scheme of the window
void size_window_set_highlight_color(SizeWindow *size_window, GColor color);

#endif