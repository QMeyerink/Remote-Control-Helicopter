//*****************************************************************************
//
// Main.c - Main program for helicopter milestone 1 project
//
//Takes analog sensor value from Helicopter rig and
// displays as a scaled value to the Orbit OLED display
//
// Author:  Quinlan Meyerink, Tyler Noah
// Last modified:   5.4.2019
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
#define MAX_DISPLAY_TICKS 10




int32_t altitude_base;
circBuf_t g_inBuffer;        // Buffer of size BUF_SIZE integers (sample values)
uint32_t g_ulSampCnt;    // Counter for the interrupts
uint8_t direction;




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
        usnprintf (line1, sizeof(line1), "                  " , altitude_base);
        usnprintf (line2, sizeof(line2), "                ");
    }

    OLEDStringDraw (line1, 0, 0);
    OLEDStringDraw (line2, 0, 1);
}

void main(void)
{

    initSystem();

    IntMasterEnable();

    int32_t altitude;
    int32_t percentage;
    int8_t display_tick;
    int8_t display_page;

    display_page = 0;
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
            if(display_page >= 2)
            {
                display_page = 0;
            }

            else
            {
                display_page++;
            }
        }

        if (checkButton (LEFT) == PUSHED)
        {
            initAltitude();
        }


        if (display_tick > MAX_DISPLAY_TICKS )
        {
        displayUpdate(altitude, percentage, display_page);
        display_tick = 0;
        } else {
        display_tick++;
        }
        //SysCtlDelay (SysCtlClockGet () / 200);
    }
}
