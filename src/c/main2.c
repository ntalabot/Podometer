/*---------------------------------------------------------------------------
Template for TP of the course "System Engineering" 2016, EPFL

Authors: Flavien Bardyn & Martin Savary
Version: 1.0
Date: 10.08.2016

---------------------------------------------------------------------------*/

#include <pebble.h>
#include "welcome_window.h"
#include "steps_window.h"
#include "distance_window.h"
#include "test_window.h"
#include "gender_window.h"

int size = 170;
int steps = 13700;
int goal = 10000;
float gender = FEMALE;

void launch_app(void *data){
  hide_welcome_window();
  show_steps_window();    // first window to show
  //show_test_window();
}

int main(void) {
    //show_welcome_window();
    //app_timer_register(2000, launch_app, NULL);    // lauches init after 2000ms

  show_steps_window();
  //show_test_window();
  
    app_event_loop();
}
