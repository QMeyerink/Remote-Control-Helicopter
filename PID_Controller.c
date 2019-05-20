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

//Three gains for controllers
//Coments next to gains show previous commit's gains
//Main Rotor
#define M_Kp 0.6 //0.4
#define M_Ki 0.0007 // 0.009
#define M_Kd 0.4 //0.8
//Tail Rotor
#define T_Kp 1 // 1
#define T_Ki 0.02 // 0.1
#define T_Kd 2.5 // 0.8

#define TAIL 0
#define MAIN 1
#define MAXIMUM_DUTY_CYCLE 98
#define MINIMUM_DUTY_CYCLE 2


static int32_t altitude_pervious_error = 0.0;
static int32_t yaw_pervious_error = 0.0;

static int32_t error_inter_main = 0.0;
static int32_t error_inter_tail = 0.0;

void PWM_rate_set(int32_t altitude_control, int32_t yaw_control)
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


    setPWM(MAIN, altitude_control);
    setPWM(TAIL, yaw_control);
}

void pid_update(int32_t altitude, int32_t altitude_setpoint, int32_t yaw, int32_t yaw_setpoint, double delta_t)
{
    int32_t altitude_error, altitude_error_deriv, altitude_control, yaw_error, yaw_error_deriv, yaw_control;

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
    altitude_control = 41 + (altitude_error*M_Kp) + (error_inter_main*M_Ki) + (altitude_error_deriv*M_Kd);
    yaw_control = (yaw_error*T_Kp) + (error_inter_tail*T_Ki) + (yaw_error_deriv*T_Kd);

    //Update the previous error
    altitude_pervious_error = altitude_error;
    yaw_pervious_error = yaw_error;

    if(altitude_error < 1 && altitude_error > -1) {
        error_inter_main = 0; //Reset inter error system
    }
    if(yaw_error < 1 && yaw_error > -1) {
        error_inter_tail = 0; //Reset inter error system
    }

    PWM_rate_set(altitude_control, yaw_control);
}

