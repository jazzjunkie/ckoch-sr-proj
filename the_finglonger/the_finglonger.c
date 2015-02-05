//*****************************************************************************
//
// freertos_demo.c - Simple FreeRTOS example.
//
// Copyright (c) 2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the EK-LM4F120XL Firmware Package.
//
//*****************************************************************************

#include <stdlib.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_timer.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"
#include "drivers/rgb.h"
#include "utils/uartstdio.h"

#include "defines.h"
#include "led_task.h"
#include "switch_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "priorities.h"
#include "linear_controller_task.h"
#include "stepper_control.h"
#include "range_finder.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>FreeRTOS Example (freertos_demo)</h1>
//!
//! This application demonstrates the use of FreeRTOS on Launchpad.
//!
//! The application blinks the user-selected LED at a user-selected frequency. 
//! To select the LED press the left button and to select the frequency press
//! the right button.  The UART outputs the application status at 115,200 baud,
//! 8-n-1 mode.
//!
//! This application utilizes FreeRTOS to perform the tasks in a concurrent
//! fashion.  The following tasks are created:
//!
//! - An LED task, which blinks the user-selected on-board LED at a
//!   user-selected rate (changed via the buttons).
//!
//! - A Switch task, which monitors the buttons pressed and passes the
//!   information to LED task.
//!
//! In addition to the tasks, this application also uses the following FreeRTOS
//! resources:
//!
//! - A Queue to enable information transfer between tasks.
//!
//! - A Semaphore to guard the resource, UART, from access by multiple tasks at
//!   the same time.
//!
//! - A non-blocking FreeRTOS Delay to put the tasks in blocked state when they
//!   have nothing to do.
//!
//! For additional details on FreeRTOS, refer to the FreeRTOS web page at:
//! http://www.freertos.org/
//
//*****************************************************************************




#define 		MAX_CMD_LEN 16
static char 	command[MAX_CMD_LEN];
extern short	target;

extern unsigned long AccelTaskInit(void);

//*****************************************************************************
//
// The mutex that protects concurrent access of UART from multiple tasks.
//
//*****************************************************************************
xSemaphoreHandle g_pUARTSemaphore;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}

#endif

static void ConsoleTask(void *pvParameters);
void HW_Init(void);

//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int
main(void)
{
	HW_Init();

    // Create a mutex to guard the UART.
    g_pUARTSemaphore = xSemaphoreCreateMutex();


#if 1
    // Create the LED task.
    if(LEDTaskInit() != 0)
        while(1);
#endif

#if 0
    // Create the switch task.
    if(SwitchTaskInit() != 0)
        while(1);
#endif

#if (MOTOR_TYPE	== DC_SLIDE)
	if(LinearControllerTaskInit() != 0)
#elif (MOTOR_TYPE == STEPPER_SCREW)
	if(StepperControllerTaskInit() != 0)
#else
#error "no motor type defined"
#endif
		while(1);


	if(AccelTaskInit() != 0)
		while(1);


	if(RangeFinderTaskInit() != 0)
	        while(1);


#if 0
	if(xTaskCreate(ConsoleTask, (signed portCHAR *)"Console", 256, NULL, tskIDLE_PRIORITY + PRIORITY_CONSOLE_TASK, NULL) != pdTRUE)
		while(1);
#endif

	// Start the scheduler.  This should not return.
    vTaskStartScheduler();

    // In case the scheduler returns for some reason, print an error and loop
    // forever.
    while(1);
}


/*
static void
ConsoleTask(void *pvParameters)
{
	//portTickType ulWakeTime;
	int arg;

	UARTwrite("\x1b[2J\x1b[1;1H>\r", 12);

	while(1)
	{
		UARTgets(command, MAX_CMD_LEN-1);

		if(strncmp(command, "seek ", 5) == 0)
		{
			arg = atoi(command + 5);

			if( (arg > 10) && (arg < 4000) )
			{
				//target = arg;
			}
		}


	}
}
*/


void HW_Init(void)
{
	// Set the clocking to run at 50 MHz from the PLL.
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	// Initialize the UART and configure it for 115,200, 8-N-1 operation.
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

	//init UART
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTStdioInit(0);

#if MOTOR_TIMER_PERIPH == SYSCTL_PERIPH_TIMER1
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF2_T1CCP0);
	GPIOPinConfigure(GPIO_PF3_T1CCP1);
#elif MOTOR_TIMER_PERIPH == SYSCTL_PERIPH_WTIMER3
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_2|GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PD2_WT3CCP0);
	GPIOPinConfigure(GPIO_PD3_WT3CCP1);
#endif

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

	//Configure timer 1 – timer is split in half; A and B timers (16 bit)
	SysCtlPeripheralEnable(MOTOR_TIMER_PERIPH);

#if RANGING_MODE == LASER_SPI
	// I2C on PD0 and PD1: jumpered to PB6 and PB7 by default, so set those as inputs to avoid conflict
	// You need to set the shared pins as input.
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	// Change the pad configuration to WPU
	GPIOPadConfigSet( GPIO_PORTB_BASE, GPIO_PIN_6 | GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD_WPU);

	//Init laser I2C
	SysCtlPeripheralEnable	(SYSCTL_PERIPH_I2C3);
	GPIOPinConfigure		(GPIO_PD0_I2C3SCL);
	GPIOPinConfigure		(GPIO_PD1_I2C3SDA);
	GPIOPinTypeI2C			(GPIO_PORTD_BASE, GPIO_PIN_1);
	GPIOPinTypeI2CSCL		(GPIO_PORTD_BASE, GPIO_PIN_0);

	I2CMasterInitExpClk		(I2C3_MASTER_BASE, SysCtlClockGet(), false);

#elif RANGING_MODE == ULTRASONIC_SPI
	// I2C on PD0 and PD1: jumpered to PB6 and PB7 by default, so set those as inputs to avoid conflict
	// You need to set the shared pins as input.
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	// Change the pad configuration to WPU
	GPIOPadConfigSet( GPIO_PORTB_BASE, GPIO_PIN_6 | GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD_WPU);

	//Init ultrasonic I2C
	SysCtlPeripheralEnable	(SYSCTL_PERIPH_I2C3);
	GPIOPinConfigure		(GPIO_PD0_I2C3SCL);
	GPIOPinConfigure		(GPIO_PD1_I2C3SDA);
	GPIOPinTypeI2C			(GPIO_PORTD_BASE, GPIO_PIN_1);
	GPIOPinTypeI2CSCL		(GPIO_PORTD_BASE, GPIO_PIN_0);

	I2CMasterInitExpClk		(I2C3_MASTER_BASE, SysCtlClockGet(), true);
#endif

	//accelerometer pin configuration
	SysCtlPeripheralEnable	(SYSCTL_PERIPH_I2C0);
	GPIOPinConfigure		(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure		(GPIO_PB3_I2C0SDA);
	GPIOPinTypeI2C			(GPIO_PORTB_BASE, GPIO_PIN_3);
	GPIOPinTypeI2CSCL		(GPIO_PORTB_BASE, GPIO_PIN_2);
	I2CMasterInitExpClk		(I2C0_MASTER_BASE, SysCtlClockGet(), true);
	GPIOPinTypeGPIOInput	(GPIO_PORTC_BASE, GPIO_PIN_7);	//interrupt pin (formerly PE2)



	//stepper driver pin configuration
	//PC4 = stepper DIR
	//PC5 = stepper STEP
	//PC6 = stepper ENABLE_N
	//PD2 = stepper MODE0
	//PD2 = stepper MODE1
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
}
