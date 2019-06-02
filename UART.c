//********************************************************
//
// UART Module for Helicopter Project abstracted from uartDemo.c - Example code for ENCE361
//Sends packets of information on the helicopters altitude, yaw, set points, flying state and
// PWM duty cycles
//UART.c
// Authors: Tyler Noah
//          Quinlan Meyerink
//

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "utils/ustdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "buttons4.h"
#include "FSM.h"
#include "PID_Controller.h"

//********************************************************
// Constants
//********************************************************
#define MAX_STR_LEN 20
//---USB Serial comms: UART0, Rx:PA0 , Tx:PA1
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX

//********************************************************
// Global variables
//********************************************************
char statusStr[MAX_STR_LEN + 1];

void initialiseUSB_UART (void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);
}

void UART_send (char *pucBuffer)
{
    // Loop while there are more characters to send.
    while(*pucBuffer)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART_USB_BASE, *pucBuffer);
        pucBuffer++;
        //use to produce string: usprintf (statusStr, "UP=%2d DN=%2d | \r\n", upPushes, downPushes);
    }
}

void UART_update(flying_state_t fly_state, int32_t yaw_goal, int32_t yaw, int32_t altitude_goal, int32_t altitude)
{
    int32_t altitude_control, yaw_control;

    altitude_control = get_altitude_control();
    yaw_control = get_yaw_control();

    char statusStr[MAX_STR_LEN + 1];
    usprintf (statusStr, "State: %2d \n\n", fly_state);
    UART_send (statusStr);
    usprintf (statusStr, "Yaw: %2d [%2d]\n\n", yaw, yaw_goal);
    UART_send (statusStr);
    usprintf (statusStr, "Alt: %2d [%2d]\n\n", altitude, altitude_goal);
    UART_send (statusStr);
    usprintf (statusStr, "PWM Main %%: %2d\n\n", altitude_control);
    UART_send (statusStr);
    usprintf (statusStr, "PWM Tail %%: %2d\n\n", yaw_control);
    UART_send (statusStr);
    usprintf (statusStr, "------------\n\n");
    UART_send (statusStr);

}
