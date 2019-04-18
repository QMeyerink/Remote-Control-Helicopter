//*****************************************************************************
//
// Main.c - Main program for helicopter milestone 2 project
//
//Takes analog sensor value from helicopter rig (PE4) and
// yaw sensor values (A = PB0, B = PB1) and displays to OLED
// the percentage altitude and yaw angle
//
// Authors:  Quinlan Meyerink & Tyler Noah
// Last modified:   18.4.2019
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

#define BUF_SIZE 25             // Size of circular buffer
#define SAMPLE_RATE_HZ 100      // Rate at which altitude is sampled
#define MAX_DISPLAY_TICKS 10    // Number of loops between OLED update.


int32_t altitude_base;         //Initial altitude value
circBuf_t g_inBuffer;          // Buffer of size BUF_SIZE integers (sample values)
uint32_t g_ulSampCnt;          // Counter for the interrupts

//Enum data type for the OLED 'page' scrolling system.
enum pages {

    perc_page,
    raw_page,
    yaw_page

};

typedef enum pages pages_t;



void displayUpdate (int32_t Altitude, int32_t Perc, pages_t displayPage, int32_t distance)
{
    // Displays on the OLED, the altitude data (raw or percentage scaled) and the yaw distance
    // depending on which page has been selected

    char line1[17]; // Display fits 16 characters wide.
    char line2[17];
    char line3[17];
    char line4[17];

    if(displayPage == 0)
    {
        usnprintf (line1, sizeof(line1), "Yaw in degrees       " );
        usnprintf (line2, sizeof(line2), "          %2d      ", distance);
        usnprintf (line3, sizeof(line1), "Altitude percent");
        usnprintf (line4, sizeof(line2), "      %3d%%      ", Perc);
    }
    else if(displayPage == 1)
    {
        usnprintf (line1, sizeof(line1), "Mean Altitude    ");
        usnprintf (line2, sizeof(line2), "      %2d        ", Altitude);
    }
    else if(displayPage == 2)
    {
        usnprintf (line1, sizeof(line1), "Yaw in degrees       " );
        usnprintf (line2, sizeof(line2), "          %2d      ", distance);
    }

    //Draw specified strings.
    OLEDStringDraw (line1, 0, 0);
    OLEDStringDraw (line2, 0, 1);
    OLEDStringDraw (line3, 0, 2);
    OLEDStringDraw (line4, 0, 3);
}

void main(void)
{
    //Run all initiate functions.
    initSystem();

    //Enable system wide interrupts
    IntMasterEnable();

    //Declare all local variables
    int32_t altitude;
    int32_t percentage;
    int8_t display_tick;
    pages_t display_page;
    int32_t distance;

    //Set initial values for local variables
    display_page = perc_page;
    display_tick = 0;

    //Full clock delay allows sensor to fill buffer
    SysCtlDelay (SysCtlClockGet ());

    //Set initial altitude value
    initAltitude();


    while(1)
    {
        //Refresh button states
        updateButtons();

        if (checkButton(UP) == PUSHED) {
            // Cycle pages on display

            if(display_page == yaw_page) {
                // Loop Pages
                display_page = perc_page;

            } else {
                //display_page++;
            }
        }

        if (checkButton(LEFT) == PUSHED) {
            // Recalibrates Altitude
            initAltitude();
        }


        //Calculate new values to be displayed
        altitude = CalcAv();
        percentage = CalcPerc(altitude);  //Scales analog average to percentage
        distance = tick_to_deg();   // Converts tick count to yaw degrees



        //Update display on every 10th main loop
        if (display_tick > MAX_DISPLAY_TICKS ) {
            displayUpdate(altitude, percentage, display_page, distance);
            display_tick = 0;

        } else {
            display_tick++;
        }
    }
}
