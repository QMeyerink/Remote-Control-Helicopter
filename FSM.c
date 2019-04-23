/*
 * FSM.c
 *
 *  Created on: 10/04/2019
 *  For ENCE 361 Helicopter Project Milestone 2
 *      Author: Quinlan Meyerink
 */
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


enum state {
    state_zero,
    state_one,
    state_two,
    state_three

};

typedef enum state state_t;
#define NUM_OF_PINS 448

state_t previous_state;
extern int32_t distance;


void direction_calculator(bool sensorA)
//De/Increments the distance value
//to reflect state change
//Takes previous state and moves to next state
//depending on which sensor changed
//Transitions are gray coded i.e only
//one sensor changes value (A)
{
    //If a full rotation has been completed set back to distance count back to 0
    if((distance == NUM_OF_PINS)||(distance == -NUM_OF_PINS)) {
        distance = 0;
    }

    switch(previous_state) {
        case 0 : // A=0, B=0
            if(sensorA) {
                previous_state = state_three;
                distance -= 1;
                break;

            } else {
                previous_state = state_one;
                distance += 1;
                break;
            }

        case 1 : // A=0, B=1
            if(sensorA) {
                previous_state = state_two;
                distance += 1;
                break;

            } else {
                previous_state = state_zero;
                distance -= 1;
                break;
            }

        case 2 : // A=1, B=1
            if(sensorA) {
                previous_state = state_three;
                distance += 1;
                break;

            } else {
                previous_state = state_one;
                distance -= 1;
                break;
            }

        case 3 : // A=1, B=0
            if(sensorA) {
                previous_state = state_two;
                distance -= 1;
                break;

            } else {
                previous_state = state_zero;
                distance += 1;
                break;
            }
    }

}


void init_state(bool sensorA, bool sensorB)
//Selects start up state from the sensor input
{
    state_t return_state;
    distance = 0;

    //States are gray-coded to allow simple transitions
    if(sensorA) {
        if(sensorB) {
            return_state = state_two; // A=1, B=1

        } else {
            return_state = state_three; // A=1, B=0
        }
    } else {
        if(sensorB) {
            return_state = state_one; // A=0, B=1

        } else {
            return_state = state_zero; // A=0, B=0
        }
    }
    previous_state = return_state;
}
