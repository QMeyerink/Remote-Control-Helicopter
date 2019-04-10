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
#include "circBufT.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"
#include "math.h"
#include "Interrupts.h"
#include "Calcs.h"
#include "FSM.h"

#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100

extern int32_t altitude_base;
extern circBuf_t g_inBuffer;
extern state_t previous_state;

void
initADC (void);

void
initClock (void);

void
initDisplay (void);

void initAltitude(void);

void
intYaw (void);

void
initSystem(void);

