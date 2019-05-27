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


void yawIntHandler (void)
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

void yawRefIntHandler(void)
{
    IntMasterDisable();
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_4); //Clear this interrupts flag
    GPIOIntDisable(GPIO_PORTC_BASE, GPIO_INT_PIN_4);

    distance = 0; //Set FSM yaw to 0 at ref
    fly_state = flying; //Move from calibration to flying state.

    IntMasterEnable();
}

void
initYaw (void)
{
    bool pin_state_A, pin_state_B;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);


        GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD); // This code doesnt make sense but program fails without it.
        GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD); // This code doesnt make sense but program fails without it.
        GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);

        // Set data direction register as output
        GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_DIR_MODE_IN);
        GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);
        GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_DIR_MODE_IN);

        // Set the int handler for this pin
        GPIOIntRegister(GPIO_PORTB_BASE, yawIntHandler);
        GPIOIntRegister(GPIO_PORTC_BASE, yawRefIntHandler);

        GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1, GPIO_BOTH_EDGES);
        GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_INT_PIN_4, GPIO_RISING_EDGE);

        //Enable this interrupt
        GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1);


        pin_state_A = (GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_0) == GPIO_INT_PIN_0);
        pin_state_B = (GPIOPinRead(GPIO_PORTB_BASE, GPIO_INT_PIN_1) == GPIO_INT_PIN_1);

        init_state(pin_state_A, pin_state_B);
}

void
yaw_calibration (void) {

    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_4); //enable interrupt for yaw calibration
    setPWM(1, 15);       //set the motors to calbration levels untill interrupt occurs.
    setPWM(0, 30);

}


