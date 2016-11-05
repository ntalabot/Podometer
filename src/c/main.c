/*---------------------------------------------------------------------------
Template for TP of the course "System Engineering" 2016, EPFL

Authors: Flavien Bardyn & Martin Savary
Version: 1.0
Date: 10.08.2016
---------------------------------------------------------------------------*/

// Include Pebble library
#include <pebble.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "integer_fft.h"
#include "welcome_window.h"
#include "steps_window.h"
#include "distance_window.h"
#include "gender_window.h"

#define X            0      //
#define Y            1      // Used for the array "datas"
#define Z            2      //
#define NUM_SAMPLES  16     // Value of num_samples
#define MAX_COUNTER  7      // Number of num_samples data_block to send for computation
#define SIZE_DATA    NUM_SAMPLES*(MAX_COUNTER+1)  // size of the data for step computation

// Declare the main window and two text layers
Window *main_window;
TextLayer *background_layer;
TextLayer *helloWorld_layer;
int size = 170;
int steps = 0;
int goal = 10000;
float gender = FEMALE;

// Given the filtered samples for the steps, returns the number of steps counted
static uint16_t step_count(short *samples){
  uint16_t num_edges = 0;
  uint16_t num_steps = 0;
  int16_t edges[SIZE_DATA];
  int16_t differences[SIZE_DATA];
  int16_t max_difference = 0;
  int16_t sign1,sign2;
 /* int16_t num_edges1 = 0;
  int16_t num_edges2 = 0;*/
  int16_t min_threshold = 5;
  int16_t max_threshold = 150;
  double max_distance_ratio = 0.3;

  // Count and store number of edges in the signal
  for (int i = 9 ; i < 19; ++i)
  {
    sign1 = samples[1+i-9]-samples[i-9];
    sign2 = samples[2+i-9]-samples[1+i-9];
    if (sign1*sign2 < 0)
    {
      edges[num_edges] = samples[1+i-9];
      num_edges += 1;
      if (i == 9){
        differences[i-9] = edges[i-9];
      }
      else{
        differences[i-9] = abs(edges[i-9] - edges[i-1-9]);
        // Store biggest difference in this data set
        if (differences[i] > max_difference && differences[i] < max_threshold)
          max_difference = differences[i];
      }
    }
  }

  for (int i = 0 ; i < num_edges ; i++){
    if (differences[i] > min_threshold && differences[i] < max_threshold && differences[i] > (int16_t)(max_distance_ratio*(double)max_difference) )
      num_steps += 1;
  }


/*
// Create edges array to stock top and bottom edges separatly
  if((num_edges % 2) == 0){    // Used to be fmod(num_edges,2)
    num_edges1 = num_edges>>1;  // >>1 is equivalent to /2
    num_edges2 = num_edges>>1;
  }
  else{
    num_edges1 = (num_edges+1)>>1;
    num_edges2 = (num_edges-1)>>1;
 }

  //short edges1[num_edges1]; ********************
  //short edges2[num_edges2]; ********************
  short sum1 = 0, sum2 = 0;

  // Separate edges in the 2 arrays
  for (int i = 0; i<num_edges-1 ; i++){
    if ((i % 2) == 0){  // Used to be fmod(i,2)
      //edges1[i/2] = edges[i]; *****************
      sum1 += edges[i];
    }
    else{
      //edges2[(i-1)/2] = edges[i]; *************
      sum2 += edges[i];
    }
  }

  short mean1, mean2, val1, val2;
  mean1 = sum1/num_edges1;
  mean2 = sum2/num_edges2;

  int fixed1, fixed2, threshold;

  fixed1 = 30000; // old value = 0.0084
  fixed2 = 150; // old value = 0.076

  int maxi = 0;
  // Check if conditions for counting steps are verified and count steps
  for (int i = 1 ; i < num_edges ; i++){
    if (i == 0){
      val1 = 0;
    }
    else{
      val1 = edges[i-1];
    }
    val2 = edges[i];
    if ((abs(mean1-mean2)>>2) > fixed1){  // >>2 is equivalent to /4
      threshold = abs(mean1-mean2)>>2;
    }
    else{
      threshold = fixed2;
    }

    if(abs(val2-val1) > threshold){
      if (abs(val2-val1) > 0.5*maxi){
        num_steps += 1;
        if (abs(val2-val1) > maxi)
          maxi = abs(val2-val1); // IMPROVE BY STORING EVERY DIFFERENCE AND THEN COMPARING TO MAXIMUM DIFFERENCE AT END
      }
    }
  }*/



  return num_steps;
}

//Function that filters given sample array and modifies it with new filtered values
static void filter_samples(short *samples) {
  short fr[SIZE_DATA], fi[SIZE_DATA];
  int M, lowerbound; // M is the power of 2 that gives us the number of samples we have. lowerbound is when we start to filter
  int HNHN = 0; ///////////////////////////////////////////////// garbage
  // Copy sample values in table and initialise fi
  //char fourier[20000];
 // char *temp = "          ";
 // strcpy(fourier, "\nTest ");

  for (int i = 0; i < SIZE_DATA ;i++){
    fr[i] = samples[i];
    fi[i] = 0;
    //APP_LOG(APP_LOG_LEVEL_DEBUG,"\n%d",samples[i]);

    //snprintf(temp,10,"%d ",samples[i]);
    //strcat(fourier, temp);
  }

//  APP_LOG(APP_LOG_LEVEL_DEBUG,"\n%s",fourier);

  // Lowerbound is ceiling of 0.05*num_samples
  switch(SIZE_DATA){
    case(32) : M = 5; lowerbound = 2;  break;
    case(64) : M = 6; lowerbound = 4;  break;
    case(128) : M = 7; lowerbound = 7;  break;
    case(254) : M = 8; lowerbound = 13; break;
  }

  // Do the fft
  HNHN = HNHN+M+1+lowerbound+fr[0]+fi[0]; // To avoid unused variable ; Only for Debugging
  fix_fft(fr,fi,M,0);

  // Filter depending on the lowerbound
  for (int i = lowerbound; i < SIZE_DATA ;i++){
    fr[i] = 0;
    fi[i] = 0;
  }

  // Do inverse fft to get filtered samples
  int scale = fix_fft(fr,fr,M,1);
    APP_LOG(APP_LOG_LEVEL_DEBUG,"\nINVERSE\n");

  // Give values back to samples array. Imaginary part is added because it seems closer to what we want
  for (int i = 0 ; i < SIZE_DATA ; i++){
    fr[i] = fr[i]<<scale;
    fi[i] = fi[i]<<scale;
    samples[i] = abs(fr[i]) + abs(fi[i]);
        //APP_LOG(APP_LOG_LEVEL_DEBUG,"\n%d",samples[i]);


  }
}

// Function that compute the number of steps in the raw_data[SIZE_DATA]
static uint16_t compute_steps(short (*raw_data)[SIZE_DATA]) {
  short i, squared_norm[SIZE_DATA]; // store the squared norm of the data

  // square norm calculation
    for (i = 0; i < SIZE_DATA ; ++i)
      squared_norm[i] = raw_data[X][i] * raw_data[X][i] +
                    raw_data[Y][i] * raw_data[Y][i] + raw_data[Z][i] * raw_data[Z][i];

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting filtering...\n");
  filter_samples(squared_norm);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting step counting...\n");
  return step_count(squared_norm);
}

// Function called when num_samples are ready from the accel.
static void accel_data_handler(AccelData *data, uint32_t num_samples) {
  uint32_t i;
  int temp_steps;
  static uint16_t total_steps = 0;
  static short counter = 0;            // used to know when we have all samples ready
  static short raw_data[3][SIZE_DATA]; // store those samples
  static char  results[60];

  APP_LOG(APP_LOG_LEVEL_DEBUG, "counter = %d\n", counter);
  // Store the data from the accel. into one big array
  for (i = 0; i < num_samples ; ++i)
  {
    raw_data[X][i+num_samples*counter] = data[i].x>>5; // >>4 is equivalent to /16
    raw_data[Y][i+num_samples*counter] = data[i].y>>5; // to avoid overflow with values that are too big
    raw_data[Z][i+num_samples*counter] = data[i].z>>5;
  }

  if (counter == MAX_COUNTER) // when all the data is ready, send it
  {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending the data...\n");
    temp_steps = compute_steps(raw_data);
    total_steps += temp_steps;
    steps = total_steps;
    snprintf(results, 60, "Temp: %d; Total: %d", temp_steps, total_steps);
    text_layer_set_text(helloWorld_layer, results);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Temp: %d; Total: %d\n", temp_steps, total_steps);
    counter = 0;
    update_steps_window();
    update_distance_window();
  }
  else
    ++counter;
  return;
}

// Change windows from welcome to steps
void launch_app(void *data){
  hide_welcome_window();
  show_steps_window();    // first window to show
}

// Init function called when app is launched
static void init(void) {
  short datar[] = {15,700,-4,0,-69,0,7,0} ;
  short datai[] = {0,0,0,0,0,0,0,0};
  fix_fft(datar, datai, 3, 0);
 // four1(data,2,0);
  for (int i = 0; i <= 7 ; i++)
    {
        APP_LOG(APP_LOG_LEVEL_INFO,"Forward %hi %hi\n", datar[i], datai[i]);

  }
  fix_fft(datar,datai, 3, 1);
  for (int i = 0; i <= 7 ; i++)
    {
        APP_LOG(APP_LOG_LEVEL_INFO,"Reverse %hi %hi\n", datar[i], datai[i]);

  }
    int num_samples = NUM_SAMPLES;
    // Subscribe to accelerometer
    accel_data_service_subscribe(num_samples, accel_data_handler);

    // Define accelerometer sampling rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);

         // Create main Window element and assign to pointer
         main_window = window_create();
    Layer *window_layer = window_get_root_layer(main_window);

                // Create background Layer
                background_layer = text_layer_create(GRect( 0, 0, 144, 168));
                // Setup background layer color (black)
                text_layer_set_background_color(background_layer, GColorBlack);

                // Create text Layer
                helloWorld_layer = text_layer_create(GRect( 20, 65, 100, 20));
                // Setup layer Information
                text_layer_set_background_color(helloWorld_layer, GColorClear);
                text_layer_set_text_color(helloWorld_layer, GColorWhite);
                text_layer_set_font(helloWorld_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
         text_layer_set_text_alignment(helloWorld_layer, GTextAlignmentCenter);
    text_layer_set_text(helloWorld_layer, "My new text");

         // Add layers as childs layers to the Window's root layer
    layer_add_child(window_layer, text_layer_get_layer(background_layer));
          layer_add_child(window_layer, text_layer_get_layer(helloWorld_layer));

         // Show the window on the watch, with animated = true
         window_stack_push(main_window, true);

    // Add a logging message (for debug)
          APP_LOG(APP_LOG_LEVEL_DEBUG, "End of init'");
}

// deinit function called when the app is closed
static void deinit(void) {

    // Destroy layers and main window
    text_layer_destroy(background_layer);
          text_layer_destroy(helloWorld_layer);
    window_destroy(main_window);
}

int main(void) {
    init();

    show_welcome_window();
    app_timer_register(2000, launch_app, NULL);    // lauches init after 2000ms

    //show_steps_window();

    app_event_loop();
  deinit();
  return EXIT_SUCCESS;
}
