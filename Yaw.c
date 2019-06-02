/*
 * Yaw.c
 *
 *  Created on: 15/05/2019
 *
 *  Contains the initialise Yaw function
 *  Along with the two interrupt handlers for calibration and yaw
 *  Also has the yaw calibration routine
 *
 *  For ENCE 361 Helicopter Project Milestone 2
 *      Author: Quinlan Meyerink, Tyler Noah
 */


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
#include "FSM.h"
#include "PWM_Module.h"

#define MAIN 1
#define TAIL 0

#define CALIBRATION_MAIN_DUTY 30
#define CALIBRATION_TAIL_DUTY 30


void yaw_int_handler (void)
{
    //Disable interrupts during interrupt
    IntMasterDisable();

    //Clear the interrupt
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1);

    bool pin_state_A;

    //Gives a high or low pin states
    pin_state_A = (GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0) == GPIO_INT_PIN_0);

    //Updates the global distance value.
    direction_calculator(pin_state_A);

    //Re-enable interrupts
    IntMasterEnable();
}


void yaw_ref_int_handler(void)
{
    //ISR for having found the Yaw reference during calibration stage of flight

    IntMasterDisable();

    //Clear and disable this interrupt
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_4);
    GPIOIntDisable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);

    yaw_ticks = 0; //Set FSM yaw to 0 at ref
    fly_state = flying; //Move from calibration to flying state.

    IntMasterEnable();
}

void init_yaw (void)
{
    //Enables and configures the three pins used for yaw
    //monitoring on the helicopter sensors A and B as well
    //as the reference sensor

    bool pin_state_A, pin_state_B;

    //Enable each of the data ports used for Yaw
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

        //Configure the port pad to be weak pull up pins
        GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);
        GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);
        GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);

        // Set data direction register as output
        GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_DIR_MODE_IN);
        GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);
        GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_DIR_MODE_IN);

        // Set the interrupt handler for this pin
        GPIOIntRegister(GPIO_PORTB_BASE, yaw_int_handler);
        GPIOIntRegister(GPIO_PORTC_BASE, yaw_ref_int_handler);

        //Set what change in signal will cause an interrupt
        GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1, GPIO_BOTH_EDGES);
        GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_INT_PIN_4, GPIO_RISING_EDGE);

        //Enable this interrupt
        GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1);

        //Check the current state of each pin
        //by checking against itself we can form a boolean value for each pin
        pin_state_A = (GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0) == GPIO_INT_PIN_0);
        pin_state_B = (GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_1) == GPIO_INT_PIN_1);

        //Set the initial state of the yaw FSm on start up
        init_state(pin_state_A, pin_state_B);
}

void yaw_calibration (void) {
    //Enables the interrupt for the yaw reference sensor on pin PC4
    //Also sets helicopter motors to a gentle rotation to find reference

    //Enable interrupt for yaw calibration
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);

    //Set rotors to set values for calibration routine
    set_PWM(MAIN, CALIBRATION_MAIN_DUTY);
    set_PWM(TAIL, CALIBRATION_TAIL_DUTY);

}


