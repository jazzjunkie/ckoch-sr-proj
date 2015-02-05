/*
 * linear_controller_task.c
 *
 *  Created on: Oct 31, 2014
 *      Author: Chris
 */

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "linear_controller_task.h"

#include "driverlib/i2c.h"

#include "priorities.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"


#include "drivers/rgb.h"

#include "defines.h"

#if (MOTOR_TYPE	== DC_SLIDE)


static unsigned short	pwm_power[2]	= { (MOTOR_PWM_PERIOD-1), (MOTOR_PWM_PERIOD-1) };
static long				power;
static long				slider_pos[4]	= { 0, 0, 0, 0 };
static unsigned char	slide_index		= 0;

long					range_bound		= 500;

char 					finger_present = 1;
char 					motor_stall	= 0;

short					target			= 2048;
MOTOR_STATE				motor_state		= motor_neutral;

void InitSlideADC( void );
unsigned long WaitI2CDone( unsigned long ulBase);

unsigned char laser_or_ultrasonic = 1;
#if ENABLE_LASER_SENSOR
volatile unsigned long long echo_time;
volatile unsigned long long echo_time_a;
volatile unsigned long long echo_time_b;
#endif

//*****************************************************************************
//
// This task toggles the user selected LED at a user selected frequency. User
// can make the selections by pressing the left and right buttons.
//
//*****************************************************************************
static void
LinearControllerTask(void *pvParameters)
{
	portTickType ulWakeTime;

	short error;

	short p_err = 0;
	short i_err = 0;
	short d_err = 0;
	short prev_err = 0;
	char temp_index;

	//short dummy_target		= 0;
	//short dummy_increment	= 2;
	//short dummy_i			= 201;

	InitSlideADC();

	TimerConfigure(MOTOR_TIMER_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);

	TimerLoadSet		(MOTOR_TIMER_BASE, TIMER_A, MOTOR_PWM_PERIOD);
	TimerLoadSet		(MOTOR_TIMER_BASE, TIMER_B, MOTOR_PWM_PERIOD);
	TimerControlLevel	(MOTOR_TIMER_BASE, TIMER_BOTH, 1);
	TimerMatchSet		(MOTOR_TIMER_BASE, TIMER_A, 1);
	TimerMatchSet		(MOTOR_TIMER_BASE, TIMER_B, 1);
	TimerEnable			(MOTOR_TIMER_BASE, TIMER_A);
	TimerEnable			(MOTOR_TIMER_BASE, TIMER_B);



	ulWakeTime = xTaskGetTickCount();
    while(1)
    {
    	slide_index &= 0x03;

    	//Get the ADC value of the slider
		ADCProcessorTrigger(ADC0_BASE, 3);								// Trigger the ADC conversion.
		vTaskDelayUntil(&ulWakeTime, MOTOR_TASK_DELAY);					// Put the task to sleep while ADC is converting

		while(!ADCIntStatus(ADC0_BASE, 3, false))						// Wait for conversion to be completed, just in case.
		{
			vTaskDelay(1);
		}
		ADCIntClear(ADC0_BASE, 3);										// Clear the ADC interrupt flag.
		ADCSequenceDataGet(ADC0_BASE, 3, (unsigned long *)(slider_pos + slide_index));	// Read ADC Value.


		error = target - (int)(slider_pos[slide_index & 0x03]);

#if 1
		if( !finger_present || motor_stall ) //determine if there's a motor problem, such as no finger or motor stall
		{
			//TODO: if it's a motor stall, wait some time to try again.
			pwm_power[0] = pwm_power[1] = 1;
		}
		else
		{
			p_err = error ;
			//p_err = (error * 3) >> 1;

			i_err += error >> 1;
			if(i_err > 800)
				i_err = 800;
			else if( i_err < -800 )
				i_err = -800;

			d_err = error - prev_err;
			//d_err = prev_err - error;

			power = ( 2.5 * p_err ) + ( 0 * i_err ) + ( d_err * 1 );

			temp_index = (power > 0) ? 0 : 1;

			power = abs(power);
			if (power > (MOTOR_PWM_PERIOD - 1) )
				power = (MOTOR_PWM_PERIOD - 1);

			pwm_power[temp_index]	= (MOTOR_PWM_PERIOD-1);
			pwm_power[1-temp_index]	= MOTOR_PWM_PERIOD - power;


			prev_err = error;
		}

		TimerMatchSet(MOTOR_TIMER_BASE, TIMER_A, pwm_power[0]);
		TimerMatchSet(MOTOR_TIMER_BASE, TIMER_B, pwm_power[1]);
#else
		//TODO: detect when the slider is not being touched and go into neutral.
		//TODO: go into neutral if the motor is not behaving.
		if(0)
		{
			motor_state	= motor_neutral;
		}
		else if(abs(error) < 10)
		{
			motor_state = motor_brake;
		}
#if 1
		else if((motor_state == motor_brake) && (slider_speed[0] > 5))
		{
			//keep braking until the motor stops.
		}
#endif
		else
		{



			//p-term is proportional to the error.
			p_term = ((abs(error) > 2000) ? 2000 : abs(error))

			//determine speed and acceleration of the slider.
			slider_speed[0] = slider_pos[slide_index & 0x03] - slider_pos[(slide_index - 1) & 0x03];
			slider_speed[1] = slider_pos[(slide_index-2) & 0x03] - slider_pos[(slide_index - 3) & 0x03];
			slider_accel	= slider_speed[0] - slider_speed[1];



		}


		switch ( motor_state )
		{
		case motor_brake:
			pwm_power[0] = (MOTOR_PWM_PERIOD-1);
			pwm_power[1] = (MOTOR_PWM_PERIOD-1);
			break;

		case motor_forward:
			pwm_power[0] = power;
			pwm_power[1] = (MOTOR_PWM_PERIOD-1);
			break;

		case motor_reverse:
			pwm_power[0] = (MOTOR_PWM_PERIOD-1);
			pwm_power[1] = power;
			break;

		case motor_neutral:
		default:
			pwm_power[0] = 1;
			pwm_power[1] = 1;
			break;
		}

		TimerMatchSet(TIMER1_BASE, TIMER_A, pwm_power[0]);
		TimerMatchSet(TIMER1_BASE, TIMER_B, pwm_power[1]);

#endif
		++slide_index;
    }
}


/*
static void
MotorTask(void *pvParameters)
{
    portTickType ulWakeTime;

    unsigned short target = 2048;

    unsigned char state = 0;

    unsigned short power = 1000;

    pwm_power[0] = 0;
    pwm_power[1] = 0;

    ulWakeTime = xTaskGetTickCount();

    //Configure timer 1 – timer is split in half; A and B timers (16 bit)
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);

	//Values and settings for Timer1(A) – this timer outputs to pf2
	TimerLoadSet(TIMER1_BASE, TIMER_A, MOTOR_PWM_PERIOD);
	TimerMatchSet(TIMER1_BASE, TIMER_A, 0); // PWM
	TimerEnable(TIMER1_BASE, TIMER_A);

	//Values and settings for Timer1(B) – this timer outputs to pf3
	TimerLoadSet(TIMER1_BASE, TIMER_B, MOTOR_PWM_PERIOD);
	TimerMatchSet(TIMER1_BASE, TIMER_B, 0); // PWM
	TimerEnable(TIMER1_BASE, TIMER_B);

    //HWREG(TIMER1_BASE + 0x0000000C)   |= 0x4040;
    TimerControlLevel(TIMER1_BASE, TIMER_BOTH, 1);

    while(1)
    {
#if 1

#else
    	if((echo_time > 3000) || (echo_time < 300))
    	{
    		TimerMatchSet(TIMER1_BASE, TIMER_A, 1);
			TimerMatchSet(TIMER1_BASE, TIMER_B, 1);
    	}
    	else
    	{
    		target = echo_time - 256;

			if( abs((long)slider_pos - (long)target) < 100 )
			{
				//if(state != 0)
				{
					state = 0;
					TimerMatchSet(TIMER1_BASE, TIMER_A, MOTOR_PWM_PERIOD-1);
					TimerMatchSet(TIMER1_BASE, TIMER_B, MOTOR_PWM_PERIOD-1);
				}
			}
			else
			{
				power = (MOTOR_PWM_PERIOD - abs((long)slider_pos - (long)target)) >> 1;

				if( slider_pos > target )
				{
					//if(state != 1)
					{
						state = 1;
						//TimerEnable(TIMER1_BASE, TIMER_A);
						//TimerDisable(TIMER1_BASE, TIMER_B);

						TimerMatchSet(TIMER1_BASE, TIMER_A, power);
						TimerMatchSet(TIMER1_BASE, TIMER_B, MOTOR_PWM_PERIOD-1);

					}
				}
				else //if(state != 2)
				{
					state = 2;
					//TimerDisable(TIMER1_BASE, TIMER_A);
					//TimerEnable(TIMER1_BASE, TIMER_B);

					TimerMatchSet(TIMER1_BASE, TIMER_A, MOTOR_PWM_PERIOD-1);
					TimerMatchSet(TIMER1_BASE, TIMER_B, power);
				}
			}
    	}

#endif
    	vTaskDelayUntil(&ulWakeTime, configTICK_RATE_HZ >> 8 );
    }
}
#endif

*/



unsigned int id;
unsigned int err;
unsigned int regadd;




unsigned long LinearControllerTaskInit(void)
{
	 if(xTaskCreate(LinearControllerTask, (signed portCHAR *)"Linear Control", MOTORTASKSTACKSIZE, NULL, tskIDLE_PRIORITY + PRIORITY_LINEAR_CONTROL_TASK, NULL) != pdTRUE)
		 return(1);

#if 0
	 if(xTaskCreate(MotorTask, (signed portCHAR *)"Motor", 256, NULL, tskIDLE_PRIORITY + 2, NULL) != pdTRUE)
		 return(1);
#endif

#if 0
	 if(xTaskCreate(UltrasonicTask, (signed portCHAR *)"Sonic", 256, NULL, tskIDLE_PRIORITY + PRIORITY_ULTRASONIC_TASK, NULL) != pdTRUE)
		 return(1);
#endif

	 //
	 // Success.
	 //
	 return(0);
}

void InitSlideADC( void )
{
	// Enable sample sequence 3 with a processor signal trigger.  Sequence 3
	// will do a single sample when the processor sends a signal to start the
	// conversion.  Each ADC module has 4 programmable sequences, sequence 0
	// to sequence 3.  This example is arbitrarily using sequence 3.
	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

	//
	// Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
	// single-ended mode (default) and configure the interrupt flag
	// (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
	// that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
	// 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
	// sequence 0 has 8 programmable steps.  Since we are only doing a single
	// conversion using sequence 3 we will only configure step 0.  For more
	// information on the ADC sequences and steps, reference the datasheet.
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);

	// Since sample sequence 3 is now configured, it must be enabled.
	ADCSequenceEnable(ADC0_BASE, 3);

	// Clear the interrupt status flag.  This is done to make sure the
	// interrupt flag is cleared before we sample.
	ADCIntClear(ADC0_BASE, 3);
}

#endif
