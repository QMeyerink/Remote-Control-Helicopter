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
#include "driverlib/uart.h"
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
#include "FSM.h"
#include "UART.h"
#include "Yaw.h"

#define BUF_SIZE 25             // Size of circular buffer
#define SAMPLE_RATE_HZ 100      // Rate at which altitude is sampled
#define MAX_DISPLAY_TICKS 100    // Number of loops between OLED update.

#define YAW_STEP 15
#define ALTITUDE_STEP 10
#define ALTITUDE_MAX 100
#define ALTITUDE_MIN 0
#define YAW_MIN -180
#define YAW_MAX 180

int32_t altitude_base;         //Initial altitude value
circBuf_t g_inBuffer;          // Buffer of size BUF_SIZE integers (sample values)
uint32_t g_ulSampCnt;          // Counter for the interrupts

extern flying_state_t fly_state;
extern int32_t altitude_control;
extern int32_t yaw_control;


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
initSystem(void)
{
    initClock ();
    initADC();
    initDisplay();
    initButtons();
    initYaw();
    initialisePWM();
    initialiseUSB_UART();
    initCircBuf(&g_inBuffer, BUF_SIZE);
}

int32_t update_altitude_goal(int32_t altitude_goal, flying_state_t fly_state)
{
    if(fly_state == landing){
        return 0;
    }

    if (checkButton(UP) == PUSHED) {
        if (altitude_goal > 90) {
            altitude_goal = ALTITUDE_MAX;
        } else {
            altitude_goal += ALTITUDE_STEP;
        }
    }

    if (checkButton(DOWN) == PUSHED) {
    //Increment altitude by -10% down to 0%
        if (altitude_goal < 10) {
            altitude_goal = ALTITUDE_MIN;
        } else {
            altitude_goal -= ALTITUDE_STEP;
        }
    }
    return altitude_goal;
}

int32_t update_yaw_goal(int32_t yaw_goal, flying_state_t fly_state)
{
    if(fly_state == landing){
        return 0;
    }

    if (checkButton(LEFT) == PUSHED) {
        ////Increment yaw by 15 deg down to -180
        if (yaw_goal < -165) {
            yaw_goal = -YAW_MIN;
        } else {
            yaw_goal -= YAW_STEP;
        }
    }

    if (checkButton(RIGHT) == PUSHED) {
        //Increment yaw by 15 deg up to +180
        if (yaw_goal > 165) {
            yaw_goal = YAW_MAX;
        } else {
            yaw_goal += YAW_STEP;
        }
    }

    return yaw_goal;
}


void main(void)
{
    //Run all initiate functions.
    initSystem();

    //Declare all local variables
    int32_t altitude, percentage, yaw, altitude_goal, yaw_goal;
    int8_t display_tick;

    //Set initial values for local variables
    display_tick = 0;
    altitude_goal = 0;
    yaw_goal = 0;
    fly_state = landed;

    //Full clock delay allows sensor to fill buffer
    SysCtlDelay (SysCtlClockGet ());

    //Set initial altitude value
    initAltitude();

    while(1)
    {
        //Refresh button states
        updateButtons();

        //Check current system state
        update_state();

        if(fly_state != landed) {
            //update the goal
            yaw_goal = update_yaw_goal(yaw_goal, fly_state);
            altitude_goal = update_altitude_goal(altitude_goal, fly_state);

            //Check if reset button has been pressed
            if(checkButton(RESET) == PUSHED) {
                SysCtlReset();
            }

            altitude = CalcAv(); //Calculates the average ADC altitude value
            percentage = CalcPerc(altitude);  //Scales analog average to percentage
            yaw = tick_to_deg();   // Converts tick count to yaw degrees

            if(fly_state != calibration) {

                pid_update(percentage, altitude_goal, yaw, yaw_goal, 1000 ); //Update control PWM signals for rotors.
            }
        }

        //Update serial on every 10th main loop
        if (display_tick > MAX_DISPLAY_TICKS ) {
            UART_update(fly_state, yaw_goal, yaw, altitude_goal, percentage, altitude_control, yaw_control);
            display_tick = 0;

        } else {
            display_tick++;
        }
    }
}
