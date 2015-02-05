/*
 * stepper_control.c
 *
 *  Created on: Jan 21, 2015
 *      Author: Chris
 */

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stepper_control.h"
#include "priorities.h"

#include "defines.h"

#if (MOTOR_TYPE == STEPPER_SCREW)

static unsigned short	step_period_ms;
//static unsigned char	direction;


volatile unsigned char	step_toggle = 0;
volatile unsigned short	current_pos	= 0;
volatile unsigned short	target_pos	= 0;

volatile unsigned short	step_count	= 0;
char step_increment = 0;

void StepTimerInterrupt(void)
{
    ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, step_toggle);
    step_toggle ^= GPIO_PIN_5;

    if( step_count )
    {
    	current_pos += step_increment;
    	--step_count;
    }
    else
    {
    	TimerIntDisable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    }
}


void SeekPos( short target, short speed )
{
	//range-check target
	if( (target >= 0) && (target < 1600) )
	{
		if(current_pos == 0)
			current_pos = target;

		target_pos = target;

		//range-check speed
		/*
		if(speed < MIN_STEP_PERIOD)
			speed = MIN_STEP_PERIOD;
		else if(speed < MAX_STEP_PERIOD)
			speed = MAX_STEP_PERIOD;
			*/

		if(abs(target_pos - current_pos) < 50)
			speed = 10;
		else
			speed = 5;

		step_period_ms = speed;
	}

	//target_pos = test_pos;
}


static void LinearControllerTask(void *pvParameters)
{
	portTickType ulWakeTime;

	step_period_ms = 10;
	target_pos = 0;
	current_pos = 0;

	char dir = 0;

	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0);

	ulWakeTime = xTaskGetTickCount();

	while(1)
	{
		//enable stepper driver


		while(abs((short)current_pos - (short)target_pos) < 5)
			vTaskDelay(2);


		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0);

#if ENABLE_DEBUG_UART_MSGS
		UARTprintf("\r\nNew target: %d (pos=%d)", target_pos, current_pos );

#endif


		while(current_pos != target_pos)
		{
			if(current_pos > target_pos)
			{
				dir = 0;
				--current_pos;
			}
			else
			{
				dir = GPIO_PIN_4;
				++current_pos;
			}

			GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, dir);

			//send one STEP pulse
			GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, GPIO_PIN_5);
			//UARTprintf(".");
			vTaskDelay(1);
			GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_5, 0);


			//TODO: temporary
			//vTaskDelay(2);
			//vTaskDelayUntil(&ulWakeTime, 7);
			vTaskDelayUntil(&ulWakeTime, step_period_ms);
		}

		//disable stepper driver
		GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_PIN_6);

#if ENABLE_DEBUG_UART_MSGS
		//UARTprintf("\tDone.", target_pos, current_pos );

#endif

		vTaskDelayUntil(&ulWakeTime, 5);
		//xSemaphoreTake()
	}
}


unsigned long StepperControllerTaskInit(void)
{
	 if(xTaskCreate(LinearControllerTask, (signed portCHAR *)"Linear Control", 256, NULL, tskIDLE_PRIORITY + PRIORITY_LINEAR_CONTROL_TASK, NULL) != pdTRUE)
		 return(1);

	 return(0);
}


void SetStepMode(STEP_MODE mode)
{
	//Note: M0 is floating by default; M1 has a 200k pull-down resistor

	switch(mode)
	{
	case FULL_STEP:
		//M0 low, M1 low
		GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2);
		GPIOPinWrite(GPIO_PORTD_BASE, (GPIO_PIN_2 | GPIO_PIN_3), 0);
		break;

	case HALF_STEP:
		//M0 high, M1 low
		GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2);
		GPIOPinWrite(GPIO_PORTD_BASE, (GPIO_PIN_2 | GPIO_PIN_3), GPIO_PIN_2 );
		break;

	case QUARTER_STEP:
		//M0 floating, M1 low
		GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_2);
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0 );
		break;

	case EIGHTH_STEP:
		//M0 low, M1 high
		GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2);
		GPIOPinWrite(GPIO_PORTD_BASE, (GPIO_PIN_2 | GPIO_PIN_3), GPIO_PIN_3 );
		break;

	case SIXTEENTH_STEP:
		//M0 high, M1 high
		GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2);
		GPIOPinWrite(GPIO_PORTD_BASE, (GPIO_PIN_2 | GPIO_PIN_3), (GPIO_PIN_2 | GPIO_PIN_3));
		break;

	case THIRTYSECOND_STEP:
		//M0 floating, M1 high
		GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_2);
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_PIN_3 );
		break;

	default:
		break;
	}
}

void SetSpeed(short speed)
{
	STEP_MODE	mode;
	short		power = MOTOR_PWM_PERIOD >> 2; //for now, fixed 25% duty cycle

	if(speed < 100)
	{
		mode = FULL_STEP;
		//power =
	}
	else if( speed < 200 )
	{
		mode = HALF_STEP;
		//power =
	}
	else if( speed < 300 )
	{
		mode = QUARTER_STEP;
		//power =
	}
	else if( speed < 400 )
	{
		mode = EIGHTH_STEP;
		//power =
	}
	else if( speed < 500 )
	{
		mode = SIXTEENTH_STEP;
		//power =
	}
	else
	{
		mode = THIRTYSECOND_STEP;
		//power =
	}

	SetStepMode(mode);
	TimerMatchSet(TIMER1_BASE, TIMER_A, power);
}

#endif
