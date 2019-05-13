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
#define M_Kp 0.5
#define M_Ki 0.009
#define M_Kd 0

#define T_Kp 2
#define T_Ki 1
#define T_Kd 1

static signed long main_pervious_error = 0.0;
static signed long tail_pervious_error = 0.0;
static signed long error_inter_main = 0.0;
static signed long error_inter_tail = 0.0;
static signed long main_PWM = 0.0;

void main_pid_update(int32_t altitude, int32_t setpoint, int32_t delta_t);

void tail_pid_update(int32_t yaw, int32_t setpoint, int32_t delta_t);


void PWM_rate_set(int32_t control);

