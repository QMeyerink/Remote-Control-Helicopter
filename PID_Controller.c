/*
 * PID_Controller.c
 *
 *  Created on: 20/04/2019
 *  Contains a PID controller with static gains
 *  A function to keep PWM duty cycle values within bounds
 *  Two functions to access the current main and rotor duty cycles.
 *
 *  For ENCE 361 Helicopter Project
 *      Author: Quinlan Meyerink, Tyler Noah
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
#include "PWM_Module.h"



//Main Rotor gains
#define M_Kp 0.3
#define M_Ki 0.0007
#define M_Kd 0.3

//Tail Rotor gains
#define T_Kp 1.0
#define T_Ki 0.0011
#define T_Kd 2.5

#define TAIL 0
#define MAIN 1
#define MAXIMUM_DUTY_CYCLE 70
#define MINIMUM_DUTY_CYCLE 2



static int32_t altitude_pervious_error = 0.0;
static int32_t yaw_pervious_error = 0.0;

static int32_t error_inter_main = 0.0;
static int32_t error_inter_tail = 0.0;

static int32_t altitude_control = 0;
static int32_t yaw_control = 0;




void PWM_rate_set()
{
    //make sure PWM signal doesnt go out of bounds

    if(altitude_control > MAXIMUM_DUTY_CYCLE) {
        altitude_control = MAXIMUM_DUTY_CYCLE;
    } else if(altitude_control < MINIMUM_DUTY_CYCLE) {
        altitude_control = MINIMUM_DUTY_CYCLE;
    }
    if(yaw_control > MAXIMUM_DUTY_CYCLE) {
        yaw_control = MAXIMUM_DUTY_CYCLE;
    } else if(yaw_control < MINIMUM_DUTY_CYCLE) {
        yaw_control = MINIMUM_DUTY_CYCLE;
    }


    set_PWM(MAIN, altitude_control);
    set_PWM(TAIL, yaw_control);
}

void pid_update(int32_t altitude, int32_t altitude_setpoint, int32_t yaw, int32_t yaw_setpoint, double delta_t)
{
    int32_t altitude_error, altitude_error_deriv, yaw_error, yaw_error_deriv;

    //Error is the difference between where we are and where we want to be.
    altitude_error = (altitude_setpoint - altitude);
    yaw_error = (yaw_setpoint - yaw);


    //Calculate the integral error
    error_inter_main = altitude_error * delta_t;
    error_inter_tail = yaw_error * delta_t;

    //Calculate the differential error
    altitude_error_deriv = (altitude_error - altitude_pervious_error) / delta_t;
    yaw_error_deriv = (yaw_error - yaw_pervious_error) / delta_t;

    //Set control level value (This will set the PWM duty-cycle)
    altitude_control = 32 + (altitude_error*M_Kp) + (error_inter_main*M_Ki) + (altitude_error_deriv*M_Kd);
    yaw_control = (yaw_error*T_Kp) + (error_inter_tail*T_Ki) + (yaw_error_deriv*T_Kd);

    //Update the previous error
    altitude_pervious_error = altitude_error;
    yaw_pervious_error = yaw_error;

    //Set the PWM for each rotor
    PWM_rate_set();
}

int32_t get_altitude_control() {
    return altitude_control;
}

int32_t get_yaw_control() {
    return yaw_control;
}

