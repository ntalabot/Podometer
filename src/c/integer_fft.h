#ifndef __INTEGER_FFT_H__
#define __INTEGER_FFT_H__

#pragma once
/* Apply the fft or the ifft on the given data
 *   fr and fi are respectively the real and imaginary part
 *   m is the size of the data in power of 2
 *   inverse = 0 to apply FFT; inverse = 1 to apply IFFT
 */
int fix_fft(short fr[], short fi[], int m, int inverse);

#endif