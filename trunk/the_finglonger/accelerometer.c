/*
 * accelerometer.c
 *
 *  Created on: Dec 8, 2014
 *      Author: Chris
 */
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"

#include "FreeRTOS.h"
#include "task.h"
#include "accelerometer.h"
#include "driverlib/i2c.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

unsigned long temp_data = 0;

unsigned char ReadRegByte(unsigned char addr)
{
	I2CMasterSlaveAddrSet( I2C0_MASTER_BASE, MPU6050_DEFAULT_ADDRESS, false);   // false = write, true = read
	I2CMasterDataPut( I2C0_MASTER_BASE, addr );
	I2CMasterControl( I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);
	while( I2CMasterBusy(I2C0_MASTER_BASE))
		vTaskDelay( 1 );

	/** Set read mode.  */
	I2CMasterSlaveAddrSet( I2C0_MASTER_BASE, MPU6050_DEFAULT_ADDRESS, true);   // false = write, true = read
	I2CMasterControl( I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

	while( I2CMasterBusy(I2C0_MASTER_BASE))
		vTaskDelay( 1 );

	return I2CMasterDataGet(I2C0_MASTER_BASE);
}

void WriteRegByte(unsigned char addr, unsigned char value)
{
	I2CMasterSlaveAddrSet( I2C0_MASTER_BASE, MPU6050_DEFAULT_ADDRESS, false);   // false = write, true = read
	I2CMasterDataPut( I2C0_MASTER_BASE, addr );
	I2CMasterControl( I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);

	while( I2CMasterBusy(I2C0_MASTER_BASE))
		vTaskDelay( 1 );

	I2CMasterDataPut( I2C0_MASTER_BASE, value );
	I2CMasterControl( I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);

	/** Set read mode.  */
	//I2CMasterSlaveAddrSet( I2C0_MASTER_BASE, MPU6050_DEFAULT_ADDRESS, true);   // false = write, true = read
	//I2CMasterControl( I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
	//vTaskDelay( 1 );
	//WaitI2CDone( I2C0_MASTER_BASE);
	while( I2CMasterBusy(I2C0_MASTER_BASE))
		vTaskDelay( 1 );

	//return I2CMasterDataGet(I2C0_MASTER_BASE);
}

short ReadRegShort(unsigned char addr)
{
	short data = 0;

	I2CMasterSlaveAddrSet( I2C0_MASTER_BASE, MPU6050_DEFAULT_ADDRESS, false);   // false = write, true = read
	I2CMasterDataPut( I2C0_MASTER_BASE, addr );
	I2CMasterControl( I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);

	while( I2CMasterBusy(I2C0_MASTER_BASE))
		vTaskDelay(1);

	I2CMasterSlaveAddrSet( I2C0_MASTER_BASE, MPU6050_DEFAULT_ADDRESS, true);   // false = write, true = read
	I2CMasterControl( I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);

	while( I2CMasterBusy(I2C0_MASTER_BASE))
		vTaskDelay(1);

	data = (short)(I2CMasterDataGet(I2C0_MASTER_BASE)) << 8;

	I2CMasterControl( I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

	while( I2CMasterBusy(I2C0_MASTER_BASE))
		vTaskDelay(1);

	data |= I2CMasterDataGet(I2C0_MASTER_BASE);

	return data;
/*
	vTaskDelay( 1 );
	//WaitI2CDone( I2C0_MASTER_BASE);
	while( I2CMasterBusy(I2C0_MASTER_BASE));


	I2CMasterSlaveAddrSet( I2C0_MASTER_BASE, MPU6050_DEFAULT_ADDRESS, true);   // false = write, true = read
	I2CMasterControl( I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
	vTaskDelay( 1 );
	//WaitI2CDone( I2C0_MASTER_BASE);
	while( I2CMasterBusy(I2C0_MASTER_BASE));

	return I2CMasterDataGet(I2C0_MASTER_BASE);
	*/
}

long global_x_accel;

long range_bound;

static void AccelTask(void *pvParameters)
{
	portTickType ulWakeTime;

	 short x_accel;
	 short y_accel;
	 short z_accel;

	//reset the "sleep" bit.
	 WriteRegByte(MPU6050_RA_PWR_MGMT_1, 0x00);

	 ulWakeTime = xTaskGetTickCount();
	while(1)
	{

		x_accel = ReadRegByte(MPU6050_RA_ACCEL_XOUT_H);
		x_accel <<= 8;
		x_accel |= ReadRegByte(MPU6050_RA_ACCEL_XOUT_L);
/*
		y_accel = ReadRegByte(MPU6050_RA_ACCEL_YOUT_H);
		y_accel <<= 8;
		y_accel |= ReadRegByte(MPU6050_RA_ACCEL_YOUT_L);

		z_accel = ReadRegByte(MPU6050_RA_ACCEL_ZOUT_H);
		z_accel <<= 8;
		z_accel |= ReadRegByte(MPU6050_RA_ACCEL_ZOUT_L);
*/
		//UARTprintf("\x1b[2J\x1b[1;1HX: %i\r\nY: %i\r\nZ: %i\r", (x_accel), (y_accel), (z_accel) );

		/*
		if(x_accel < -100)
		{
			//level orientation has X value of 0, pointing straight down has a value of roughly -16384 -1*(2^14).
			//That represents 1g. 2g is -1*(2^15).
			range_bound;
		}
		*/

		global_x_accel = x_accel;

		if(x_accel > 0)
			x_accel = 0;
		else if(x_accel < -16500)
			x_accel = -16500;

		//when pointed straight down (-16500), max the range at 100cm distance and level (0) at 250?
		range_bound = 250 + (x_accel * 0.009 );



		vTaskDelayUntil(&ulWakeTime, 50);
	}
}

unsigned long AccelTaskInit(void)
{
	if(xTaskCreate(AccelTask, (signed portCHAR *)"Accelerometer", 256, NULL, tskIDLE_PRIORITY + 1, NULL) != pdTRUE)
			 return(1);

	return 0;
}
