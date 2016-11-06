#ifndef __STEPS_WINDOW_H__
#define __STEPS_WINDOW_H__

// Display the steps window on the Pebble
void show_steps_window(void);

// Ask for an update of the steps window to display current values
void update_steps_window(void);

// Hide the steps window, so that another window can be displayed
void hide_steps_window(void);

#endif