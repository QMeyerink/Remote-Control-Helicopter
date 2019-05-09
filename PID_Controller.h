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
#define M_Kp 1.5
#define M_Ki 3
#define M_Kd 0

#define T_Kp 1
#define T_Ki 0
#define T_Kd 0

static double main_pervious_error = 0.0;
static double tail_pervious_error = 0.0;

void main_pid_update(double altitude, double setpoint, double delta_t);

void tail_pid_update(double yaw, double setpoint, double delta_t);


void PWM_rate_set(double control);

