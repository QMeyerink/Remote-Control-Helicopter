#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "utils/ustdlib.h"
#include "circBufT.h"
#include "driverlib/debug.h"

extern circBuf_t g_inBuffer;
extern uint32_t g_ulSampCnt;


void SysTickIntHandler(void);

void ADCIntHandler(void);

void initADC (void);
