/*
 * stepper_control.h
 *
 *  Created on: Jan 25, 2015
 *      Author: Chris
 */

#ifndef STEPPER_CONTROL_H_
#define STEPPER_CONTROL_H_

#define STEPPER_M0_PIN	GPIO_PIN_2
#define STEPPER_M1_PIN	GPIO_PIN_3
#define STEPPER_M0_BANK	GPIO_PORTD_BASE
#define STEPPER_M1_BANK	GPIO_PORTD_BASE

typedef enum
{
	FULL_STEP,
	HALF_STEP,
	QUARTER_STEP,
	EIGHTH_STEP,
	SIXTEENTH_STEP,
	THIRTYSECOND_STEP
} STEP_MODE;

void SetStepMode(STEP_MODE mode);
void SetSpeed(short speed);


void SeekPos( short target, short speed );
unsigned long StepperControllerTaskInit(void);

#endif /* STEPPER_CONTROL_H_ */
