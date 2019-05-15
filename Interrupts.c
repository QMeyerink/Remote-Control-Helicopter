/*
 * Interrupts.h
 *
 *  Created on: 4/04/2019
 *  For ENCE 361 Helicopter Project Milestone 2
 *      Author: Quinlan Meyerink, Tyler Noah
 */

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


extern int32_t distance;
extern flying_state_t fly_state;
extern circBuf_t g_inBuffer;
extern uint32_t g_ulSampCnt;


void SysTickIntHandler(void)
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
void ADCIntHandler(void)
{
    //Disable interrupts during this interrupt
    IntMasterDisable();

    //Clear the interrupt
    ADCIntClear(ADC0_BASE, 3);

    uint32_t ulValue;
    //
    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    //
    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);

    //Re-enable interrupts
    IntMasterEnable();
}

void yawIntHandler (void)
{
    //Disable interrupts during interrupt
    IntMasterDisable();

    //Clear the interrupt
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1);

    bool pin_state_A;

    //Gives a high or low pin states
    pin_state_A = (GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0) == GPIO_INT_PIN_0);

    //Updates the global distance value.
    direction_calculator(pin_state_A);

    //Re-enable interrupts
    IntMasterEnable();
}

void yawRefIntHandler(void)
{
    IntMasterDisable();

    GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_4);

    distance = 0; //Set FSM yaw to 0 at ref
    fly_state = flying; //Move from calibration to flying state.
    GPIOPinWrite(GPIO_PORTF_BASE,  GPIO_PIN_3, GPIO_PIN_3);
    IntMasterEnable();

}



