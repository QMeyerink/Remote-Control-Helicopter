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

#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100

extern int32_t altitude_base;
extern circBuf_t g_inBuffer;

void
initADC (void)
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
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    //
    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);
}

void
initClock (void)
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
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}


void
initDisplay (void)
{
  // Intialise the Orbit OLED display
    OLEDInitialise ();
}

void initAltitude(void)
{
    //Calibrates the current mean ADC reading to be the Base Altitude

    altitude_base = CalcAv();

}

void
initYawA (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

        // Set up the specific port pin as medium strength current & pull-down config.
        // Refer to TivaWare peripheral lib user manual for set up for configuration options
        GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);

        // Set data direction register as output
        GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_DIR_MODE_IN);

        GPIOIntRegister(GPIO_PORTB_BASE, yawIntHandler);

        GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0);

        // Enable GPIO Port F
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Set up for the green_LED (Used for testing only)

            // Set up the specific port pin as medium strength current & pull-down config.
            // Refer to TivaWare peripheral lib user manual for set up for configuration options
            GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);

            // Set data direction register as output
            GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_DIR_MODE_OUT);

            // Write a zero to the output pin 3 on port F
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);

}


void
initSystem(void)
{
    initClock ();
    initADC();
    initDisplay();
    initButtons();
    initYawA();
    initCircBuf(&g_inBuffer, BUF_SIZE);
}
