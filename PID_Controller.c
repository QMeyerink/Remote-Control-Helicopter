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
#define M_Kp 1//0.9
#define M_Ki 0.5
#define M_Kd 0

#define T_Kp 1.1
#define T_Ki 0.01
#define T_Kd 2.8

static int32_t main_pervious_error = 0.0;
static int32_t tail_pervious_error = 0.0;
static int32_t error_inter_main = 0.0;
static int32_t error_inter_tail = 0.0;
static int32_t main_PWM = 0.0;

void PWM_rate_set(int32_t control, int8_t rotor)
{
    //make sure PWM signal doesnt go out of bounds

    if(control > 98) {
        control = 98;
    } else if(control < 2) {
        control = 2;
    }

    setPWM(rotor, control);
}

void main_pid_update(int32_t altitude, int32_t setpoint, double delta_t)
{
    int32_t error, error_deriv;

    //Error is the difference between where we are and where we want to be.
    error = (setpoint - altitude);

    //integrate and differentiate with respect to time.
    error_inter_main += error;
    error_deriv = (error - main_pervious_error) / delta_t;

    //Set control level value (This will set the PWM duty-cycle)
    main_PWM = 10 + (error*M_Kp) + (error_inter_main*M_Ki) + (error_deriv*M_Kd);

    //Update the previous error
    main_pervious_error = error;

    if(error == 0) {
        error_inter_main = 0; //Reset inter error system
    }

    PWM_rate_set(main_PWM, 1);
}

void tail_pid_update(int32_t yaw, int32_t setpoint, double delta_t)
{
    int32_t error, error_deriv, control;

    //Error is the difference between where we are and where we want to be.
    error = (setpoint - yaw);

    //integrate and differentiate with respect to time.
    error_inter_tail = error;
    error_deriv = (error - tail_pervious_error) / delta_t;

    //Set control level value (This will set the PWM duty-cycle)
    control = (error*T_Kp) + (error_deriv*T_Kd) + (error_inter_tail*T_Ki);

    //Update the previous error
    tail_pervious_error = error;

    if(error == 0) {
        error_inter_tail = 0; //Reset inter error system
    }

    PWM_rate_set(control, 0);
}


