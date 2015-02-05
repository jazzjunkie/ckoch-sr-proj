/*
 * range_finder.h
 *
 *  Created on: Jan 25, 2015
 *      Author: Chris
 */

#ifndef RANGE_FINDER_H_
#define RANGE_FINDER_H_

unsigned long RangeFinderTaskInit(void);

#define CMD_REG 0
#define CMD_VAL 1

#define LIDAR_REG_CMD_CTRL 0x00
#define LIDAR_CMD_ACQ	4
#define LIDAR_REG16_DISTANCE 0x8F

typedef enum
{
	LIDAR_CMD_SIMPLE_RANGE	= 0,
	LIDAR_NUM_COMMANDS		= 1
} LIDAR_COMMAND;



#endif /* RANGE_FINDER_H_ */
