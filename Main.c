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
#include "Interrupts.h"
#include "Init.h"
#include "Calcs.h"

#define BUF_SIZE 25
#define SAMPLE_RATE_HZ 100


int32_t ALTITUDE_BASE;
circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
uint32_t g_ulSampCnt;    // Counter for the interrupts




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

    initSystem();

    IntMasterEnable();

    int8_t displayPage;
    int32_t altitude;
    int32_t percentage;
    int8_t display_tick;

    displayPage = 0;
    display_tick = 0;

    SysCtlDelay (SysCtlClockGet ());

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
