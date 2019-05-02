/*
 * Calcs.c
 *
 *
 *  Created on: 4/04/2019
 *  For ENCE 361 Helicopter Project Milestone 2
 *      Author: Quinlan Meyerink, Tyler Noah
 */

#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "circBufT.h"
#include "math.h"


#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100
#define PINS_TO_DEG_RATIO 360/448
#define ANALOG_RANGE 1092


extern int32_t altitude_base;
extern circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
int32_t distance;


int32_t CalcAv(void)
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

int32_t CalcPerc(int32_t Average)
{

    //Find the rounded percentage value of altitude
    //relative to inital altitude or set altitude
    int32_t percent = floor(((Average - altitude_base) * -100) / ANALOG_RANGE);
    return percent;
}

int32_t tick_to_deg(void)
{
    //Convert The amount of pins sensor has passed to degrees
    //relative to initial position.
    return(distance*PINS_TO_DEG_RATIO);
}
