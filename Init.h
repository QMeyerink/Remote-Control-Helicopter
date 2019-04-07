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

extern circBuf_t g_inBuffer;
extern int32_t altitude_base;

#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100

void
initADC (void);

void
initClock (void);

void
initDisplay (void);

void initAltitude(void);

void
intYawA (void);

void
intYawB (void);

void
initSystem(void);

