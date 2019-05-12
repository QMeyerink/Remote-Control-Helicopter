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

//Three gains for controllers (These are just random values i put in, they have no actual reasoning behind them)
#define M_Kp 0.0004
#define M_Ki 1
#define M_Kd 0

#define T_Kp 2
#define T_Ki 1
#define T_Kd 0

static double main_pervious_error = 0.0;
static double tail_pervious_error = 0.0;
static double current_main_PWM = 0.0;

void PWM_rate_set(double control, int8_t rotor)
{
    //Transform the 'control' value to a duty cycle
    if(rotor == 1){
        current_main_PWM += control;

        if(current_main_PWM > 70) {
            current_main_PWM = 70;
        } else if(current_main_PWM < 38) {
            current_main_PWM = 38;
        }
        setPWM(rotor, current_main_PWM);
    }
    //Use set_pwm() to set new duty cycle.
    if(control > 70) {
        control = 70;
    } else if(control < 10) {
        control = 10;
    }

    if(rotor == 0) {
        setPWM(rotor, control);
    }
}

void main_pid_update(double altitude, double setpoint, double delta_t)
{
    double error, error_inter, error_deriv, control;

    //Error is the difference between where we are and where we want to be.
    error = (setpoint - altitude);

    //integrate and differentiate with respect to time.
    error_inter = error * delta_t;
    error_deriv = (error - main_pervious_error) / delta_t;

    //Set control level value (This will set the PWM duty-cycle)
    control = (error*M_Kp) + (error_inter*M_Ki) ;// + (error_deriv*M_Kd) + (error_inter*M_Ki);

    main_pervious_error = error;

    PWM_rate_set(control, 1);
}

void tail_pid_update(double yaw, double setpoint, double delta_t)
{
    double error, error_inter, error_deriv, control;

    //Error is the difference between where we are and where we want to be.
    error = (setpoint - yaw);

    //integrate and differentiate with respect to time.
    error_inter = error * delta_t;
    error_deriv = (error - tail_pervious_error) / delta_t;

    //Set control level value (This will set the PWM duty-cycle)
    control = (error*T_Kp) + (error_deriv*T_Kd) + (error_inter*T_Ki);

    tail_pervious_error = error;

    PWM_rate_set(control, 0);
}


