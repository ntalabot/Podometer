/*---------------------------------------------------------------------------
Pedometer for TP of the course "System Engineering" 2016, EPFL

Authors: Léa Bole-Feysot, Quentin Golay, Benjamin Ramtoula & Nicolas Talabot
Version: 1.0
Date: 06.11.2016
---------------------------------------------------------------------------*/

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

#define X            0      // Used for the array "raw_data"
#define Y            1      //
#define Z            2      //
#define NUM_SAMPLES  16     // Value used for num_samples
#define MAX_COUNTER  7      // Number of num_samples data_blocks to send for steps computation, minus 1
#define SIZE_DATA    NUM_SAMPLES*(MAX_COUNTER+1)  // size of the data for steps computation : 16 * (7+1) = 128

// Global variables used for the person's informations
int size = 170;
int steps = 0;
int goal = 10000;
float gender = FEMALE;

// Given the filtered norm of the acceleration, returns the number of steps counted
static uint16_t step_count(short *samples) {
	uint16_t num_edges = 0;
	uint16_t num_steps = 0;
	int16_t edges[SIZE_DATA];
	int16_t differences[SIZE_DATA];
	int16_t max_difference = 0;
	int16_t sign1, sign2;
	int16_t min_threshold = 200;
	int16_t max_threshold = 1000;
	float max_distance_ratio = 0.6;

	// Count and store number of edges (extremums) in the signal
	for (int i = 0; i < 10; ++i)
  {
    // Check if there is a sign change to see if the point is a local extremum
    // The 9 sample points of indexes 9 to 17 are used because they are where
    // we find our stable filtered data following the FFT.
		sign1 = samples[i + 9] - samples[i + 8];
		sign2 = samples[i + 10] - samples[i + 9];
		if (sign1*sign2 < 0) // Local extremum found
		{
			edges[num_edges] = samples[i + 9];
			num_edges++;
			if (num_edges == 1) // If this is the first edge of our filtered signal, we don't consider it
				differences[num_edges-1] = 0;
			else { // Otherwise we take the difference between it and the previous edge
				differences[num_edges-1] = abs(edges[num_edges-1] - edges[num_edges - 2]);
				// Store biggest difference in this data set
				if ((differences[num_edges-1] > max_difference) && (differences[num_edges-1] < max_threshold))
					max_difference = differences[num_edges-1];
			}
		}
	}

  // Count the steps based on the value of the difference between the edges of the signal
	for (int i = 0; i < num_edges; i++) {
    // Difference should be between minimum and maximum thresholds, and not too small compared to the biggest difference found
		if ((differences[i] > min_threshold) && (differences[i] < max_threshold) && (differences[i] > (int16_t)(max_distance_ratio*(float)max_difference)))
			num_steps += 1;
	}
  // Return 2 times the calculated number to try to compensate the problems with FFT
	return 2*num_steps;
}

/*Function that low-pass filters given sample array.
First it finds the fast fourier transform, then puts the values
of frequencies higher than the lowerbound to 0, and computes the inverse fourier
transform. This gives the filter sample array */
static void filter_samples(short *samples) {
	short fr[SIZE_DATA], fi[SIZE_DATA]; // used for the fft and ifft
	int M, lowerbound; // M is the power of 2 that gives us the number of samples we have. lowerbound is when we start to filter

	// Copy sample values in table and initialise fi
	for (int i = 0; i < SIZE_DATA; i++) {
		fr[i] = samples[i];
		fi[i] = 0;
	}

	// Lowerbound is 0.05 * SIZE_DATA rounded up and depends on the size of the data
	switch (SIZE_DATA) {
	case(32) : M = 5; lowerbound = 2;  break;
	case(64) : M = 6; lowerbound = 4;  break;
	case(128) : M = 7; lowerbound = 7;  break;
	case(254) : M = 8; lowerbound = 13; break;
	}

	// Do the fft
	fix_fft(fr, fi, M, 0);

	// Filter depending on the lowerbound
	for (int i = lowerbound; i < SIZE_DATA; i++) {
		fr[i] = 0;
		fi[i] = 0;
	}

	// Do inverse fft to get filtered samples
	int scale = fix_fft(fr, fr, M, 1);

	// Give values back to samples array. Imaginary part is added because it seems closer to what we want
	for (int i = 0; i < SIZE_DATA; i++) {
		fr[i] = fr[i] << scale;
		fi[i] = fi[i] << scale;
		samples[i] = abs(fr[i]) + abs(fi[i]); // an aproximate complex modulus
	}
}

// Function that computes the number of steps in the raw_data signal given by the accelerometer
static uint16_t compute_steps(short(*raw_data)[SIZE_DATA]) {
	short i, squared_norm[SIZE_DATA];

	// Squared norm computation
	for (i = 0; i < SIZE_DATA; ++i)
		squared_norm[i] = raw_data[X][i] * raw_data[X][i] +
		                  raw_data[Y][i] * raw_data[Y][i] + raw_data[Z][i] * raw_data[Z][i];

	filter_samples(squared_norm);

	return step_count(squared_norm);
}

// Function called when num_samples are ready from the accel.
static void accel_data_handler(AccelData *data, uint32_t num_samples) {
	int i;
	static short counter = 0;            // used to know when we have all samples ready
	static short raw_data[3][SIZE_DATA]; // store those samples

	// Store the data from the accel. into one big array
	for (i = 0; i < (int)num_samples; ++i)
	{
		raw_data[X][i + num_samples*counter] = data[i].x >> 5; // >>5 is equivalent to /32
		raw_data[Y][i + num_samples*counter] = data[i].y >> 5; // to avoid overflow with values that are too big
		raw_data[Z][i + num_samples*counter] = data[i].z >> 5; // when computing the squared norm
	}

	if (counter == MAX_COUNTER) // when all the data is ready, count the steps
	{
		steps += compute_steps(raw_data);
		counter = 0;
		update_steps_window();
		update_distance_window();
	}
	else
		++counter;
	return;
}

// Change windows from welcome to steps
void launch_app(void *data) {
	hide_welcome_window();
	show_steps_window();    // We start the app on the steps window
}

// Init function called when app is launched
static void init(void) {
	// Subscribe to accelerometer
	accel_data_service_subscribe(NUM_SAMPLES, accel_data_handler);

	// Define accelerometer sampling rate
	accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
}

// Deinit function called when the app is closed
static void deinit(void) {
  // Unsubsribe to accelerometer
  accel_data_service_unsubscribe();
}

int main(void) {
	init();

	show_welcome_window();
	app_timer_register(2000, launch_app, NULL); // 2 seconds on the welcome windows, then goes to the app

  app_event_loop();

	deinit();
	return EXIT_SUCCESS;
}
