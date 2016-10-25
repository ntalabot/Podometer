/*---------------------------------------------------------------------------
Template for TP of the course "System Engineering" 2016, EPFL

Authors: Flavien Bardyn & Martin Savary
Version: 1.0
Date: 10.08.2016

Use this "HelloWorld" example as basis to code your own app, which should at least 
count steps precisely based on accelerometer data. 

- Add the accelerometer data acquisition
- Implement your own pedometer using these data
- (Add an estimation of the distance travelled)

- Make an effort on the design of the app, try to do something fun!
- Comment and indent your code properly!
- Try to use your imagination and not google (we already did it, and it's disappointing!)
  to offer us a smart and original solution of pedometer

Don't hesitate to ask us questions.
Good luck and have fun!
---------------------------------------------------------------------------*/

// Include Pebble library
#include <pebble.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "integer_fft.h"

// Declare the main window and two text layers
Window *main_window;
TextLayer *background_layer;
TextLayer *helloWorld_layer;

/************************************************
* FFT code from the book Numerical Recipes in C *
* Visit www.nr.com for the licence.             *
************************************************/

// The following line must be defined before including math.h to correctly define M_PI
#define _USE_MATH_DEFINES
#define M_PI        3.14159265358979323846264338327950288   /* pi */

#define PI      M_PI    /* pi to machine precision, defined in math.h */
#define TWOPI   (2.0*PI)

/*
 FFT/IFFT routine. (see pages 507-508 of Numerical Recipes in C)

 Inputs:
        data[] : array of complex* data points of size 2*NFFT+1.
                data[0] is unused,
                * the n'th complex number x(n), for 0 <= n <= length(x)-1, is stored as:
                        data[2*n+1] = real(x(n))
                        data[2*n+2] = imag(x(n))
                if length(Nx) < NFFT, the remainder of the array must be padded with zeros

        nn : FFT order NFFT. This MUST be a power of 2 and >= length(x).
        isign:  if set to 1, 
                                computes the forward FFT
                        if set to -1, 
                                computes Inverse FFT - in this case the output values have
                                to be manually normalized by multiplying with 1/NFFT.
 Outputs:
        data[] : The FFT or IFFT results are stored in data, overwriting the input.
*/
/*
void four1(double data[], int nn, int isign)
{
    int n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    double tempr, tempi;
    
    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
        if (j > i) {
            tempr = data[j];     data[j] = data[i];     data[i] = tempr;
            tempr = data[j+1]; data[j+1] = data[i+1]; data[i+1] = tempr;
        }
        m = n >> 1;
        while (m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    mmax = 2;
    while (n > mmax) {
        istep = 2*mmax;
        theta = TWOPI/(isign*mmax);
        wtemp = sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for (m = 1; m < mmax; m += 2) {
            for (i = m; i <= n; i += istep) {
                j =i + mmax;
                tempr = wr*data[j]   - wi*data[j+1];
                tempi = wr*data[j+1] + wi*data[j];
                data[j]   = data[i]   - tempr;
                data[j+1] = data[i+1] - tempi;
                data[i] += tempr;
                data[i+1] += tempi;
            }
            wr = (wtemp = wr)*wpr - wi*wpi + wr;
            wi = wi*wpr + wtemp*wpi + wi;
        }
        mmax = istep;
    }
}
*/

// Given the filtered samples for the steps, returns the number of steps counted
uint16_t step_count(short steps[]){
  uint16_t num_samples = sizeof(steps)/sizeof(steps);
  uint16_t num_edges = 0;
  uint16_t num_steps = 0;
  short edges[num_samples]; // used to be = {};
  int16_t sign1,sign2;
  int16_t num_edges1 = 0;
  int16_t num_edges2 = 0; 
  // Count and store number of edges in the signal
  for (int i = 0 ; i < num_samples -3 ; ++i){
    sign1 = steps[1+i]-steps[i];
    sign2 = steps[2+i]-steps[1+i];
    if (sign1*sign2 < 0){
      edges[num_edges] = steps[1+i];
      num_edges += 1;
    }
  }
  
  // Create edges array to stock top and bottom edges separatly
  if(fmod(num_edges,2) == 0){
    num_edges1 = num_edges/2;
    num_edges2 = num_edges/2;    // NICO need to declare the variables before so i moved the declarations
  }
  else{
    num_edges1 = (num_edges+1)/2;
    num_edges2 = (num_edges-1)/2; // NICO ditto
 }

  short edges1[num_edges1];
  short edges2[num_edges2];
  short sum1 = 0, sum2 = 0;

  // Separate edges in the 2 arrays
  for (int i = 0; i<num_edges-1 ; i++){
    if (fmod(i,2) == 0){
      edges1[i/2] = edges[i]; 
      sum1 += edges[i];
    }
    else{
      edges2[(i-1)/2] = edges[i];
      sum2 += edges[i];
    }
  }
           
  short mean1, mean2, val1, val2;
  mean1 = sum1/num_edges1;
  mean2 = sum2/num_edges2;

  float fixed1, fixed2, threshold; // NOT SURE CLOUD PEBBLE LIKES FLOATS...

  fixed1 = 0.0084;
  fixed2 = 0.076;


  // Check if conditions for counting steps are verified and count steps
  for (i = 1 ; i < num_edges ; i++){
    if (i == 0){
      val1 = 0; 
    }
    else{
      val1 = edges[i-1];
    }
    val2 = edges[i];
    if (fabs(mean1-mean2)*0.25 > fixed1){
      threshold = fabs(mean1-mean2)*0.25;
    }
    else{
      threshold = fixed2;
    }

    if(fabs(val2-val1) > threshold){
      num_steps += 1;
    }
  }
  return num_steps;
}

static void accel_data_handler(AccelData *data, uint32_t num_samples){
  // Read samples values
  int16_t x = data[9].x;
  int16_t y = data[9].y;
  int16_t z = data[9].z;
  
 /* float xf = x/10;
  float yf = y/10;
  float zf = z/10;*/
  
  // Create string to store results
  static char results[60];
  
  // Log results

  // Calculate acceleration norm
  int16_t acc2 = (x*x+y*y+z*z);
  
  
  // Can't print floats!!!
//  APP_LOG(APP_LOG_LEVEL_INFO,"x: %f, y: %f, z: %f \n %d",xf,yf,zf, acc2);

  // Print results
  snprintf(results,60,"Acc : %d",acc2);
  text_layer_set_text(helloWorld_layer, results);
  
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
    int num_samples = 10;
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

  
    app_event_loop();
    deinit();
}