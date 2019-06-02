/*
 * Yaw.h
 *
 *  Created on: 15/05/2019
 *
 *  Contains the initialise Yaw function
 *  Along with the two interrupt handlers for calibration and yaw
 *  Also has the yaw calibration routine
 *
 *  For ENCE 361 Helicopter Project Milestone 2
 *      Author: Quinlan Meyerink, Tyler Noah
 */

#ifndef Yaw_H_
#define Yaw_H_

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
#include "FSM.h"
#include "PWM_Module.h"


void yaw_int_handler (void);

void yaw_ref_int_handler(void);

void init_yaw (void);

void yaw_calibration (void);


#endif /*Yaw_h*/
