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

extern int32_t altitude_control;
extern int32_t yaw_control;

static int32_t error_inter_main = 0.0;
static int32_t error_inter_tail = 0.0;

void PWM_rate_set();

void pid_update(int32_t altitude, int32_t altitude_setpoint, int32_t yaw, int32_t yaw_setpoint, double delta_t);


