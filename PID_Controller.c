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

//Three gains for controllers (These are just random values i put in, they have no actual reasoning behind them)
#define Kp 1.5
#define Ki 2
#define Kd 1

static double pervious_error = 0.0

double pid_update(double altitude, double setpoint, double delta_t)
{
    double error, error_inter, error_deriv, control;

    //Error is the difference between where we are and where we want to be.
    error = setpoint - altitude;

    //integrate and differentiate with respect to time.
    error_inter = error * delta_t;
    error_deriv = (error - pervious_error) / delta_t;

    //Set control level value (This will set the PWM duty-cycle)
    control = (error*Kp) + (error_deriv*Kd) + (error_inter*Ki);

    pervious_error = error;

    return control;
}


void PWM_rate_set(double control)
{
    //Transform the 'control' value to a duty cycle

    //Use set_pwm() to set new duty cycle.
}
