/*
 * FSM.h
 *
 *  Created on: 10/04/2019
 *      Author: Quinlan Meyerink
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
    state_one = 1,
    state_two = 2,
    state_three = 3,
    state_four = 4

}current_state;


typedef enum state state_t;


state_t state_calculator(uint32_t sensorA, uint32_t sensorB);


#endif /* FSM_H_ */
