#ifndef __GENDER_WINDOW_H__
#define __GENDER_WINDOW_H__

#define GENDER_WINDOW_NUM_ROWS     2
#define GENDER_WINDOW_CELL_HEIGHT  44
#define GENDER_WINDOW_BUTTON_RADIUS 6

#define FEMALE 0.413        // For distance calculation
#define MALE 0.415

// Push the window onto the stack
void gender_window_push();

// Pop the window off the stack
void gender_window_pop ();

#endif