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
#include "Calcs.h"
#include "PWM_Module.h"
#include "PID_Controller.h"
#include "FSM.h"
#include "UART.h"
#include "Yaw.h"
#include "ADC.h"

#define BUF_SIZE 25             // Size of circular buffer
#define SAMPLE_RATE_HZ 100      // Rate at which altitude is sampled
#define MAX_DISPLAY_TICKS 100    // Number of loops between OLED update.

#define YAW_STEP 15
#define ALTITUDE_STEP 10
#define ALTITUDE_MAX 100
#define ALTITUDE_MIN 0
#define YAW_MIN -180
#define YAW_MAX 180
#define MAIN 1
#define TAIL 0
#define OFF 0
#define ATREF 0

extern flying_state_t fly_state;
extern uint32_t g_kernal_counter;

void initSystem(void)
{
    initClock ();
    initADC();
    initButtons();
    initYaw();
    initialisePWM();
    initialiseUSB_UART();
    initCircBuf(&g_inBuffer, BUF_SIZE);
    OLEDInitialise ();

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
    int32_t altitude, percentage, yaw, altitude_goal, yaw_goal, altitude_base, kernal_counter_tmp;

    //Set initial values for local variables
    altitude_goal = 0;
    yaw_goal = 0;
    kernal_counter_tmp = 0;
    fly_state = landed;

    //Full clock delay allows sensor to fill buffer
    SysCtlDelay (SysCtlClockGet ());
    altitude_base = CalcAv();

    g_kernal_counter = 0;

    while(1)
    {
        update_state();

        while(fly_state != landed) {

            //Set the current counter to a value so it doesn't change during loop
            kernal_counter_tmp = g_kernal_counter;
            update_state();
            updateButtons();

            //Check if reset button has been pressed
            if(checkButton(RESET) == PUSHED) {
                SysCtlReset();
            }
            //Updating at 100 Hz thus counter must be larger than 1
            //if (kernal_counter_tmp >= 1) {

                yaw_goal = update_yaw_goal(yaw_goal, fly_state);
                altitude_goal = update_altitude_goal(altitude_goal, fly_state);

                altitude = CalcAv(); //Calculates the average ADC altitude value
                percentage = CalcPerc(altitude, altitude_base);  //Scales analog average to percentage
                yaw = tick_to_deg();   // Converts tick count to yaw degrees

                if(fly_state != calibration) {
                    pid_update(percentage, altitude_goal, yaw, yaw_goal, 1000 ); //Update control PWM signals for rotors.
                }
            //}

            //display should be updated at 50 Hz thus counter must be larger than 2
            //if(kernal_counter_tmp >= 2) {
                //update_display
            //}

            //Updating UART at 10 Hz so counter must be larger than 10
            //if(kernal_counter_tmp >= 2) {
                UART_update(fly_state, yaw_goal, yaw, altitude_goal, percentage);
            //}

            //set the counter to the difference between current count and count occuring during loop
            g_kernal_counter -= kernal_counter_tmp;

            if(fly_state == landing) {
                if(percentage == ATREF && yaw == ATREF) {
                    setPWM(TAIL,OFF);
                    setPWM(MAIN,OFF);
                    fly_state = landed;
                }
            }
        }
    }
}
