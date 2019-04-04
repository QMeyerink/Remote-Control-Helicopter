#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "circBufT.h"
#include "math.h"


#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100


extern int32_t ALTITUDE_BASE;
extern circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
extern int16_t ANALOG_MIN = 910;
extern int16_t ANALOG_MAX = 2370; // need to change from hard code to 0.8v higher than BASE

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
    int16_t ANALOG_MIN = 910;  // Shouldnt exist
    int16_t ANALOG_MAX = 2370; // need to change from hard code to 0.8v higher than BASE
    int16_t ANALOG_RANGE = ANALOG_MAX - ANALOG_MIN;
    int32_t percent = floor(((Average - ALTITUDE_BASE) * -100) / ANALOG_RANGE);
    return percent;
}
