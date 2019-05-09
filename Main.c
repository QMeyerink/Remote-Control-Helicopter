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
#include "PWM_Module.h"
#include "PID_Controller.h"

#define BUF_SIZE 25             // Size of circular buffer
#define SAMPLE_RATE_HZ 100      // Rate at which altitude is sampled
#define MAX_DISPLAY_TICKS 10    // Number of loops between OLED update.

int32_t altitude_base;         //Initial altitude value
circBuf_t g_inBuffer;          // Buffer of size BUF_SIZE integers (sample values)
uint32_t g_ulSampCnt;          // Counter for the interrupts



void displayUpdate (int32_t altitudeGoal, int32_t yawGoal)
{
    // Displays on the OLED, the altitude data (raw or percentage scaled) and the yaw distance
    // depending on which page has been selected

    char line1[17]; // Display fits 16 characters wide.
    char line2[17];
    char line3[17];
    char line4[17];

    usnprintf (line1, sizeof(line1), "Yaw Goal:          " );
    usnprintf (line2, sizeof(line2), "       %2d         ", yawGoal); //Checks goals to test
    usnprintf (line3, sizeof(line1), "Altitude Goal:     ");
    usnprintf (line4, sizeof(line2), "      %3d%%        ", altitudeGoal); //Checks goals to test

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

    //Declare all local variables
    int32_t altitude, percentage, yaw, altitude_goal, yaw_goal, delta_t;
    int8_t display_tick;

    //Set initial values for local variables
    display_tick = 0;
    altitude_goal = 0;
    yaw_goal = 0;
    delta_t = 0;

    //Full clock delay allows sensor to fill buffer
    SysCtlDelay (SysCtlClockGet ());

    //Set initial altitude value
    initAltitude();


    while(1)
    {
        //Refresh button states
        updateButtons();

        //Check state of buttons
        if (checkButton(UP) == PUSHED) {
            //Increment altitude by +10% up to 100%
            if (altitude_goal > 90) {
                altitude_goal = 100;
            } else {
                altitude_goal += 10;
                }
            }

        if (checkButton(DOWN) == PUSHED) {
            //Increment altitude by -10% down to 0%
            if (altitude_goal < 10) {
                altitude_goal = 0;
            } else {
                altitude_goal -= 10;
            }
        }

        if (checkButton(LEFT) == PUSHED) {
            ////Increment yaw by 15 deg down to -180
            if (yaw_goal < -165) {
                yaw_goal = -180;
            } else {
                yaw_goal -= 15;
            }
        }

        if (checkButton(RIGHT) == PUSHED) {
            //Increment yaw by 15 deg up to +180
            if (yaw_goal > 165) {
                yaw_goal = 180;
            } else {
                yaw_goal += 15;
            }
        }

        //Calculate new values to be displayed
        altitude = CalcAv();
        percentage = CalcPerc(altitude);  //Scales analog average to percentage
        yaw = tick_to_deg();   // Converts tick count to yaw degrees

        delta_t = g_ulSampCnt - delta_t;

       // main_pid_update(percentage, altitude_goal, delta_t/100);
        setPWM(1, 52);
        tail_pid_update(yaw, yaw_goal, delta_t/100);


        //Update display on every 10th main loop
        if (display_tick > MAX_DISPLAY_TICKS ) {
            displayUpdate(altitude_goal, yaw_goal);
            display_tick = 0;

        } else {
            display_tick++;
        }
    }
}
