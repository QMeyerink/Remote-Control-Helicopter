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
#include "FSM.h"



extern circBuf_t g_inBuffer;
extern uint32_t g_ulSampCnt;
extern uint8_t direction;
extern state_t previous_state;


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
    IntMasterDisable();

    uint32_t ulValue;
    ADCIntClear(ADC0_BASE, 3);
    //
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    //
    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);
    //
    // Clean up, clearing the interrupt

    IntMasterEnable();
}

void
yawIntHandler (void)
{

    IntMasterDisable();

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1);


    uint32_t pin_state_A, pin_state_B;
    state_t next_state;

    pin_state_A = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0);
    pin_state_B = GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_1);

    next_state = state_calculator(pin_state_A, pin_state_B);

    if((next_state > previous_state)||((next_state == state_one) && (previous_state == state_four))) {
        direction = 1;

    } else {
        direction = 0;
    }

    previous_state = next_state;


    //TESTING CODE WITH LEDS

    if (direction)  //Using green LED for testing (Will remove once interrupt function correctly)
    {
        GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_3, GPIO_PIN_3);
        GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_1, 0x00);

    } else {
        GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_1, GPIO_PIN_1);
        GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_3, 0x00);

    }

    IntMasterEnable();
}




