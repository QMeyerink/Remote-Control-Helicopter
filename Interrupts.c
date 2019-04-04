#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "circBufT.h"



extern circBuf_t g_inBuffer;
extern uint32_t g_ulSampCnt;

void
SysTickIntHandler(void)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC0_BASE, 3);
    g_ulSampCnt++;
}

//*****************************************************************************
//
// The handler for the ADC conversion complete interrupt.
// Writes to the circular buffer.
//
//*****************************************************************************
void
ADCIntHandler(void)
{
    uint32_t ulValue;

    //
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    //
    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);
    //
    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

void
yawIntHandler (void)
{
    uint32_t pinState;

    pinState = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0);

    if (pinState)  //Using green LED for testing (Will remove once interrupt function correctly)
    {
    GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_3, GPIO_PIN_3);
    } else {
    GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_3, 0x00);
    }


   GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0);
}



