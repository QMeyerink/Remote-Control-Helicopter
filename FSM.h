/*
 * FSM.h
 *
 *  Created on: 10/04/2019
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

enum state {
    state_one,
    state_two,
    state_three,
    state_four

};


typedef enum state state_t;


uint8_t direction_calculator(bool sensorA, bool sensorB);

void init_state(bool sensorA, bool sensorB);


#endif /* FSM_H_ */
