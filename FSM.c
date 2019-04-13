/*
 * FSM.c
 *
 *  Created on: 10/04/2019
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


void direction_calculator(bool sensorA, bool sensorB)
{
    //If a full rotation has been completed set back to distance count back to 0
    if((distance == NUM_OF_PINS)||(distance == -NUM_OF_PINS)) {
        distance = 0;
    }

    //Takes previous state and moves to next state
    //depending on current sensor values
    //Also incriments the distance value
    switch(previous_state) {
        case 0 :
            if(sensorA) {
                previous_state = state_three;
                distance -= 1;
                break;

            } else {
                previous_state = state_one;
                distance += 1;
                break;
            }

        case 1 :
            if(sensorA) {
                previous_state = state_two;
                distance += 1;
                break;

            } else {
                previous_state = state_zero;
                distance -= 1;
                break;
            }

        case 2 :
            if(sensorA) {
                previous_state = state_three;
                distance += 1;
                break;

            } else {
                previous_state = state_one;
                distance -= 1;
                break;
            }

        case 3 :
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

//Selects start up state from the sensor input
void init_state(bool sensorA, bool sensorB)
{
    state_t return_state;
    distance = 0;

    if(sensorA) {
        if(sensorB) {
            return_state = state_two;

        } else {
            return_state = state_three;
        }
    } else {
        if(sensorB) {
            return_state = state_one;

        } else {
            return_state = state_zero;
        }
    }
    previous_state = return_state;
}
