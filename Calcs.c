/*
 * Calcs.c
 *
 *
 *  Created on: 4/04/2019
 *  For ENCE 361 Helicopter Project Milestone 2
 *  Performs conversion calculations for altitude and yaw values
 *      Authors: Quinlan Meyerink, Tyler Noah
 */

#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "circBufT.h"
#include "math.h"

#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100
#define PINS_TO_DEG_RATIO 0.8036
#define ANALOG_RANGE 1092

extern circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
int32_t yaw_ticks;                   // Number of state changes from reference


int32_t calc_av(void)
{
    // Calculates the Average ADC value by reading
    // a circular buffer that is filled in interrupts

    int8_t i;
    int32_t sum;
    int32_t result;

    i = 0;
    sum = 0;

    for(i = 0; i < BUF_SIZE; i++)

        sum = sum + readCircBuf (&g_inBuffer);

    result = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;

    return result;
}


int32_t calc_perc(int32_t average, int32_t altitude_base)
{
    //Find the rounded percentage value of altitude
    //relative to initial altitude or set altitude

    int32_t percent = floor(((average - altitude_base) * -100) / ANALOG_RANGE);
    return percent;
}


int32_t tick_to_deg(void)
{
    //Convert The amount of pins sensor has passed to degrees
    //relative to initial position.
    return(yaw_ticks*PINS_TO_DEG_RATIO);
}
