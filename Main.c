//*****************************************************************************
//
// Main.c - Main program for helicopter project
//
//Takes analog sensor value from helicopter rig (PE4) and
// yaw sensor values (A = PB0, B = PB1) and displays to OLED
// the percentage altitude and yaw angle
//
// Authors:  Quinlan Meyerink & Tyler Noah
// Last modified:   2.6.2019
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

#define DELTA_T 1000

#define MAIN 1
#define TAIL 0
#define OFF 0
#define ATREF 0

#define PID_KERNAL_TICKS 1
#define OLED_KERNAL_TICKS 2
#define SERIAL_KERNAL_TICKS 20

extern flying_state_t fly_state;
extern uint32_t g_kernal_counter;

void init_system(void)
{
    init_clock ();
    init_ADC();
    initButtons();
    init_yaw();
    init_PWM();
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

void display_update (int32_t altitude_goal, int32_t yaw_goal)
{
    // Displays on the OLED, the altitude data (raw or percentage scaled) and the yaw distance
    // depending on which page has been selected

    char line1[17]; // Display fits 16 characters wide.
    char line2[17];
    char line3[17];
    char line4[17];

    usnprintf (line1, sizeof(line1), "Flying state:          " );
    usnprintf (line2, sizeof(line2), "       %2d         ", fly_state); //Checks goals to test
    usnprintf (line3, sizeof(line1), "Altitude Goal:     ");
    usnprintf (line4, sizeof(line2), "      %3d%%        ", altitude_goal); //Checks goals to test

    //Draw specified strings.
    OLEDStringDraw (line1, 0, 0);
    OLEDStringDraw (line2, 0, 1);
    OLEDStringDraw (line3, 0, 2);
    OLEDStringDraw (line4, 0, 3);

}


void main(void)
{
    //Run all initiate functions.
    init_system();


    //Declare all local variables
    int32_t altitude, percentage, yaw, altitude_goal, yaw_goal, altitude_base;
    int32_t OLED_time, serial_time, PID_time;

    //Set initial values for local variables
    altitude_goal = 0;
    yaw_goal = 0;
    fly_state = landed;

    //Full clock delay allows sensor to fill buffer
    SysCtlDelay (SysCtlClockGet ());
    altitude_base = calc_av();

    g_kernal_counter = 0;
    OLED_time = 0;
    serial_time = 0;
    PID_time = 0;

    while(1)
    {
        update_state();

        while(fly_state != landed) {

            //Set the current counter to a value so it doesn't change during loop
            update_state();
            updateButtons();

            //Check if reset button has been pressed
            if(checkButton(RESET) == PUSHED) {
                SysCtlReset();
            }

            yaw_goal = update_yaw_goal(yaw_goal, fly_state);
            altitude_goal = update_altitude_goal(altitude_goal, fly_state);

            //If time between this check and last function is more than 0.01s then run (100Hz);
            if((g_kernal_counter - PID_time) >= PID_KERNAL_TICKS ){

                PID_time = g_kernal_counter;

                altitude = calc_av(); //Calculates the average ADC altitude value
                percentage = calc_perc(altitude, altitude_base);  //Scales analog average to percentage
                yaw = tick_to_deg();   // Converts tick count to yaw degrees

                if(fly_state != calibration) {
                    pid_update(percentage, altitude_goal, yaw, yaw_goal, DELTA_T ); //Update control PWM signals for rotors.
                }
            }

            //If time between this check and last function is more than 0.02s then run (50Hz)
            if((g_kernal_counter - OLED_time) >= OLED_KERNAL_TICKS) {
                OLED_time = g_kernal_counter;
                display_update (altitude_goal, yaw_goal);
            }

            //If time between this check and last function is more than 0.05s then run (20Hz)
            if((g_kernal_counter - serial_time) >= SERIAL_KERNAL_TICKS) {
                serial_time = g_kernal_counter;
                UART_update(fly_state, yaw_goal, yaw, altitude_goal, percentage);
            }



            if(fly_state == landing) {
                if(percentage == ATREF && yaw == ATREF) {
                    set_PWM(TAIL,OFF);
                    set_PWM(MAIN,OFF);
                    fly_state = landed;
                }
            }
        }
    }
}
