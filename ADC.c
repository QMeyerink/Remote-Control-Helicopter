/*
 * ADC.c
 *
 *
 *  Created on: 4/04/2019
 *  For ENCE 361 Helicopter Project Milestone 2
 *  Contains ADC interrupt handler for altitude sensor and
 *   SysTick interrupt handler for scheduling
 *      Authors: Quinlan Meyerink, Tyler Noah
 */

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

circBuf_t g_inBuffer;
uint32_t g_ulSampCnt;
uint32_t g_kernal_counter;

#define SAMPLE_RATE_HZ 100      // Rate at which altitude is sampled


void sys_tick_int_handler(void)
{
    //
    // Initiate a conversion
    //
    ADCProcessorTrigger(ADC0_BASE, 3);
    g_ulSampCnt++;
    g_kernal_counter++;
}


void ADC_int_handler(void)
{
    //Gets value from sensor and places in circular buffer

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



void init_ADC (void)
{
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);

    //
    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    //
    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADC_int_handler);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}


void init_clock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(sys_tick_int_handler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

