/*
 * defines.h
 *
 *  Created on: Jan 25, 2015
 *      Author: Chris
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#define MOTORTASKSTACKSIZE        256         // Stack size in words

#define MOTOR_PWM_PERIOD 2048
#define MOTOR_TASK_DELAY (configTICK_RATE_HZ / 200)

#define DC_SLIDE		1
#define STEPPER_SCREW	2
#define MOTOR_TYPE		STEPPER_SCREW

#define LASER_SPI		1
#define ULTRASONIC_SPI	2
#define LASER_PWM		3
#define ULTRASONIC_PWM	4

#define RANGING_MODE	LASER_SPI

//NOTE: delay is set in milliseconds
#define STEPPER_TASK_DELAY 50

#define SONIC_TASK_DELAY (configTICK_RATE_HZ / 10)

//20 full steps per 3mm, 63mm total travel
#define MAX_TARGET_POSITION (20 * 20)

#define MIN_STEP_PERIOD	(10)
#define MAX_STEP_PERIOD (200)

#define ENABLE_DEBUG_UART_MSGS 0


#define LIDAR_SPI_ADDR (0x62)

#endif /* DEFINES_H_ */
