/*
 * linear_controller_task.h
 *
 *  Created on: Oct 31, 2014
 *      Author: Chris
 */

#ifndef LINEAR_CONTROLLER_TASK_H_
#define LINEAR_CONTROLLER_TASK_H_

#define MOTOR_TIMER_PERIPH	SYSCTL_PERIPH_WTIMER3
#define MOTOR_TIMER_BASE	WTIMER3_BASE
//#define MOTOR_TIMER_PERIPH	SYSCTL_PERIPH_TIMER1
//#define MOTOR_TIMER_BASE	TIMER1_BASE

extern unsigned long LinearControllerTaskInit(void);



typedef enum
{
	motor_neutral,
	motor_forward,
	motor_reverse,
	motor_brake
} MOTOR_STATE;

#endif /* LINEAR_CONTROLLER_TASK_H_ */
