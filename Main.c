//*****************************************************************************
//
// Main.c - Main program for helicopter milestone 1 project
//
//Takes analog sensor value from Helicopter rig and
// displays as a scaled value to the Orbit OLED display
//
// Author:  Quinlan Meyerink, Tyler Noah
// Last modified:   20.3.2019
//

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

#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100


static int32_t ALTITUDE_BASE;
static circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
static uint32_t g_ulSampCnt;    // Counter for the interrupts


// *******************************************************

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

//*****************************************************************************
// Initialisation functions for the clock (incl. SysTick), ADC, display
//*****************************************************************************
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
initDisplay (void)
{
  // Intialise the Orbit OLED display
    OLEDInitialise ();
}


//*****************************************************************************
//
// Function to display the mean interval in usec
//
//*****************************************************************************

int32_t CalcAv(void)
{
    // Calculates the Average ADC value by reading
    // a circular buffer that is filled in interrupts

    int8_t i;
    int32_t sum;
    int32_t result;

    i = 0;
    sum = 0;
    for(i = 0; i < BUF_SIZE; i++)
        sum = sum + readCircBuf (&g_inBuffer);
    result = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;

    return result;
}

int32_t CalcPerc(int32_t Average)
{
    int16_t ANALOG_MIN = 910;
    int16_t ANALOG_MAX = 2370;
    int16_t ANALOG_RANGE = ANALOG_MAX - ANALOG_MIN;
    int32_t percent = floor(((Average - ALTITUDE_BASE) * -100) / ANALOG_RANGE);
    return percent;
}

void initAltitude(void)
{
    //Calibrates the current mean ADC reading to be the Base Altitude

    ALTITUDE_BASE = CalcAv();

}

void displayUpdate (int32_t Altitude, int32_t Perc, uint8_t displayPage)
{
    // Displays on the OLED, the altitude data (raw or percentage scaled)
    // depending on which page has been selected

    char line1[17]; // Display fits 16 characters wide.
    char line2[17];

    if(displayPage == 0)
    {
        usnprintf (line1, sizeof(line1), "Altitude percent");
        usnprintf (line2, sizeof(line2), "      %3d%%      ", Perc);
    }
    else if(displayPage == 1)
    {
        usnprintf (line1, sizeof(line1), "Mean Altitude");
        usnprintf (line2, sizeof(line2), "      %2d        ", Altitude);
    }
    else if(displayPage == 2)
    {
        usnprintf (line1, sizeof(line1), "                  " , ALTITUDE_BASE);
        usnprintf (line2, sizeof(line2), "                ");
    }

    OLEDStringDraw (line1, 0, 0);
    OLEDStringDraw (line2, 0, 1);
}

void main(void)
{

    initClock ();
    initADC();
    initDisplay ();
    initButtons ();
    initCircBuf (&g_inBuffer, BUF_SIZE);

    IntMasterEnable();

    int8_t displayPage;
    int32_t altitude;
    int32_t percentage;
    int8_t display_tick;

    SysCtlDelay (SysCtlClockGet ());

    displayPage = 0;
    display_tick = 0;


    initAltitude();

    while(1)
    {
        updateButtons();

        altitude = CalcAv();
        percentage = CalcPerc(altitude);

        if (checkButton (UP) == PUSHED)
        {
            if(displayPage >= 2)
            {
                displayPage = 0;
            }

            else
            {
                displayPage++;
            }
        }

        if (checkButton (LEFT) == PUSHED)
        {
            initAltitude();
        }


        if (display_tick > 10 )
        {
        displayUpdate(altitude, percentage, displayPage);
        display_tick = 0;
        } else {
        display_tick++;
        }
        //SysCtlDelay (SysCtlClockGet () / 200);
    }
}
