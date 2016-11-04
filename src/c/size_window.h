#include <pebble.h>

#define NUM_CELLS 1    // -> aussi changer dans layer.h le nombre max de cells
#define MAX_VALUE 250

typedef struct {
  int digits[NUM_CELLS];
} PIN;

typedef void (*SizeWindowComplete)(PIN pin, void *context);

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

/*
 * Creates a new PinWindow in memory but does not push it into view
 *  pin_window_callbacks: callbacks for communication
 *  returns: a pointer to a new PinWindow structure
 */
SizeWindow* size_window_create(SizeWindowCallbacks size_window_callbacks);

/*
 * Destroys an existing PinWindow
 *  pin_window: a pointer to the PinWindow being destroyed
 */
void size_window_destroy(SizeWindow *size_window);

/*
 * Push the window onto the stack
 *  pin_window: a pointer to the PinWindow being pushed
 *  animated: whether to animate the push or not
 */
void size_window_push(SizeWindow *size_window, bool animated);

/*
 * Pop the window off the stack
 *  pin_window: a pointer to the PinWindow to pop
 *  animated: whether to animate the pop or not
 */
void size_window_pop(SizeWindow *size_window, bool animated);

/*
 * Gets whether it is the topmost window or not
 *  pin_window: a pointer to the PinWindow being checked
 *  returns: a boolean indicating if it is the topmost window
 */
bool size_window_get_topmost_window(SizeWindow *size_window);

/*
 * Sets the over-all color scheme of the window
 *  color: the GColor to set the highlight to
 */
void size_window_set_highlight_color(SizeWindow *size_window, GColor color);
