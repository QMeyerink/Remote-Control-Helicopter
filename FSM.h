/*
 * FSM.h
 *
 *  Created on: 10/04/2019
 *  For ENCE 361 Helicopter Project Milestone 2
 *      Author: Quinlan Meyerink, Tyler Noah
 */

#ifndef FSM_H_
#define FSM_H_

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

enum yaw_state {
    state_one,
    state_two,
    state_three,
    state_four

};

enum flying_state {
    landed = 0,
    calibration,
    flying,
    landing,

};

typedef enum yaw_state yaw_state_t;
typedef enum flying_state flying_state_t;
#define NUM_OF_PINS 448

yaw_state_t previous_state;
flying_state_t fly_state;
extern int32_t distance;


void direction_calculator(bool sensorA);

void init_state(bool sensorA, bool sensorB);

void update_state(void);

#endif /* FSM_H_ */
