/*
 * Calcs.h
 *
 *
 *  Created on: 4/04/2019
 *  For ENCE 361 Helicopter Project
 *  Performs conversion calculations for altitude and yaw values
 *      Author: Quinlan Meyerink, Tyler Noah
 */


#ifndef Calcs_H_
#define Calcs_H_

#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "circBufT.h"
#include "math.h"


#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100


extern int32_t altitude_base;
extern circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
int32_t yaw_ticks;


int32_t calc_av(void);

int32_t calc_perc(int32_t average, int32_t altitude_base);

int32_t tick_to_deg(void);


#endif /*Calcs_H_*/
