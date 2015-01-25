/*
 * stepper_control.c
 *
 *  Created on: Jan 21, 2015
 *      Author: Chris
 */

#include "FreeRTOS.h"
#include "task.h"


void SeekPos( short target, short speed )
{

}


unsigned long StepperControllerTaskInit(void)
{
	 if(xTaskCreate(StepperControllerTask, (signed portCHAR *)"Linear Control", MOTORTASKSTACKSIZE, NULL, tskIDLE_PRIORITY + PRIORITY_LINEAR_CONTROL_TASK, NULL) != pdTRUE)
		 return(1);

#if 0
	 if(xTaskCreate(MotorTask, (signed portCHAR *)"Motor", 256, NULL, tskIDLE_PRIORITY + 2, NULL) != pdTRUE)
		 return(1);
#endif


	 //
	 // Success.
	 //
	 return(0);
}


static void
LinearControllerTask(void *pvParameters)
{
	portTickType ulWakeTime;


	while(1)
	{

		vTaskDelayUntil(&ulWakeTime, MOTOR_TASK_DELAY);
	}


}
