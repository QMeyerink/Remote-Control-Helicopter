/*
 * Interrupts.h
 *
 *  Created on: 4/04/2019
 *  For ENCE 361 Helicopter Project Milestone 2
 *      Author: Quinlan Meyerink, Tyler Noah
 */

#ifndef Interrupts_H_
#define Interrupts_H_


#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"
#include "FSM.h"

extern circBuf_t g_inBuffer;
extern uint32_t g_ulSampCnt;



void
SysTickIntHandler(void);

void
ADCIntHandler(void);

void
yawIntHandler (void);

#endif /*Interrupts_h*/
