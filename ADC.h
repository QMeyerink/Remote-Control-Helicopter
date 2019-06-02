/*
 * ADC.h
 *
 *
 *  Created on: 4/04/2019
 *  For ENCE 361 Helicopter Project Milestone 2
 *  Contains ADC interrupt handler for altitude sensor and
 *   SysTick interrupt handler for scheduling
 *  For ENCE 361 Helicopter Project
 *
 *      Author: Quinlan Meyerink, Tyler Noah
 */


#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "driverlib/debug.h"

circBuf_t g_inBuffer;
uint32_t g_ulSampCnt;

#define SAMPLE_RATE_HZ 100      // Rate at which altitude is sampled

void sys_tick_int_handler(void);

void ADC_int_handler(void);

void init_ADC (void);

void init_clock (void);

#endif /*ADC_H_*/
