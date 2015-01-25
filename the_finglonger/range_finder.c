/*
 * range_finder.c
 *
 *  Created on: Jan 21, 2015
 *      Author: Chris
 */


#include "driverlib/gpio.h"
#include "inc/hw_timer.h"
#include "driverlib/i2c.h"

#define SONIC_TASK_DELAY (configTICK_RATE_HZ / 10)


#if (RANGING_MODE == LASER_PWM) || (RANGING_MODE == ULTRASONIC_PWM)
void EchoInterrupt()
{
	if(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2))
	{
		echo_time_a = TimerValueGet64(WTIMER0_BASE);
	}
	else
	{
		echo_time_b = TimerValueGet64(WTIMER0_BASE);
		echo_time = echo_time_a - echo_time_b;
#if RANGING_MODE == LASER_PWM
		echo_time >>= 9;
#elif RANGING_MODE == ULTRASONIC_PWM
		echo_time >>= 8;
#endif
	}
	GPIOPinIntClear(GPIO_PORTA_BASE, GPIO_PIN_2);
}
#endif

/* This function returns:
 * I2C_MASTER_ERR_NONE, I2C_MASTER_ERR_ADDR_ACK, I2C_MASTER_ERR_DATA_ACK, or
 * I2C_MASTER_ERR_ARB_LOST.
 */
unsigned long WaitI2CDone( unsigned long ulBase)
{
    // Wait until done transmitting
    while( I2CMasterBusy(ulBase));
    	//vTaskDelay(1);
    // Return I2C error code
    return I2CMasterErr( ulBase);
}

static void UltrasonicTask(void *pvParameters)
{
	unsigned long distance;
	unsigned long range;
	unsigned long smoothed_distance = 100;

	portTickType ulWakeTime;


	//I2CMasterSlaveAddrSet	(I2C3_MASTER_BASE, 0xE0, false);

	//I2CMasterIntEnable		(I2C3_MASTER_BASE);
	//I2CIntRegister			(I2C3_MASTER_BASE, EchoInterrupt);

#if RANGING_MODE == LASER_SPI

	I2CMasterEnable			(I2C3_MASTER_BASE);

/*
	I2CMasterSlaveAddrSet( I2C3_MASTER_BASE, (0xC4 / 2), false);   // false = write, true = read
	I2CMasterDataPut( I2C3_MASTER_BASE, 0x4A);
	I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	vTaskDelay(1);
	WaitI2CDone(I2C3_MASTER_BASE);
	I2CMasterDataPut( I2C3_MASTER_BASE, 0x10);
	I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
	vTaskDelay(1);
	WaitI2CDone(I2C3_MASTER_BASE);
*/
#elif RANGING_MODE == ULTRASONIC_SPI

#elif RANGING_MODE == LASER_PWM
	//for now, always enable laser rangefinder
	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);

#elif RANGING_MODE == ULTRASONIC_PWM
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);

	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2);
	GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_BOTH_EDGES);
	GPIOPortIntRegister(GPIO_PORTA_BASE, EchoInterrupt);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
	TimerConfigure(WTIMER0_BASE, TIMER_CFG_ONE_SHOT);
	TimerPrescaleSet(WTIMER0_BASE, TIMER_BOTH, 0);

	TimerEnable(WTIMER0_BASE, TIMER_BOTH);
	GPIOPinIntEnable(GPIO_PORTA_BASE, GPIO_PIN_2);
#else

#endif

	SysCtlDelay(10000);


	ulWakeTime = xTaskGetTickCount();
	while(1)
	{
	#if RANGING_MODE == LASER_SPI

		//do {
			I2CMasterSlaveAddrSet( I2C3_MASTER_BASE, (0xC4 / 2), false);   // false = write, true = read
			I2CMasterDataPut( I2C3_MASTER_BASE, 0x00);
			I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);
			vTaskDelay(1);
			WaitI2CDone(I2C3_MASTER_BASE);
			I2CMasterDataPut( I2C3_MASTER_BASE, 0x04);
			I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
			vTaskDelay(1);
			err |= WaitI2CDone(I2C3_MASTER_BASE);
		//} while HWREG(I2C3_MASTER_BASE + I2C_O_MCS);
		vTaskDelay(100);

		I2CMasterSlaveAddrSet( I2C3_MASTER_BASE, (0xC4 / 2), false);   // false = write, true = read
		I2CMasterDataPut( I2C3_MASTER_BASE, 0x8F );
		I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);
		vTaskDelay(1);
		err |= WaitI2CDone(I2C3_MASTER_BASE);

		I2CMasterSlaveAddrSet( I2C3_MASTER_BASE, (0xC4 / 2), true);   // false = write, true = read
		I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
		vTaskDelay(1);
		err |= WaitI2CDone(I2C3_MASTER_BASE);
		id = I2CMasterDataGet(I2C3_MASTER_BASE) << 8;
		I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		vTaskDelay(1);
		err |= WaitI2CDone(I2C3_MASTER_BASE);
		id |= I2CMasterDataGet(I2C3_MASTER_BASE);

		/*
		I2CMasterSlaveAddrSet( I2C3_MASTER_BASE, (0xC4 / 2), false);   // false = write, true = read
		I2CMasterDataPut( I2C3_MASTER_BASE, 0x0F );
		I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);
		vTaskDelay(1);
		while( I2CMasterBusy(I2C3_MASTER_BASE));

		I2CMasterSlaveAddrSet( I2C3_MASTER_BASE, (0xC4 / 2), true);   // false = write, true = read
		I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
		vTaskDelay(1);
		while( I2CMasterBusy(I2C3_MASTER_BASE));
		id = I2CMasterDataGet(I2C3_MASTER_BASE) << 8;

		I2CMasterSlaveAddrSet( I2C3_MASTER_BASE, (0xC4 / 2), false);   // false = write, true = read
		I2CMasterDataPut( I2C3_MASTER_BASE, 0x10 );
		I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);
		vTaskDelay(1);
		while( I2CMasterBusy(I2C3_MASTER_BASE));

		I2CMasterSlaveAddrSet( I2C3_MASTER_BASE, (0xC4 / 2), true);   // false = write, true = read
		I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
		vTaskDelay(1);
		while( I2CMasterBusy(I2C3_MASTER_BASE));
		id |= I2CMasterDataGet(I2C3_MASTER_BASE);
*/

		UARTprintf("\x1b[2J\x1b[1;1H%d\r", id );

		vTaskDelayUntil(&ulWakeTime, 100);

	#elif RANGING_MODE == ULTRASONIC_SPI
		 /** Send register address. */
		I2CMasterSlaveAddrSet( I2C3_MASTER_BASE, 112, false);   // false = write, true = read
		// Set register address of AK8963
		I2CMasterDataPut( I2C3_MASTER_BASE, 81);
		// Start sending data
		I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);


		vTaskDelayUntil(&ulWakeTime, /*SONIC_TASK_DELAY*/ 100  );
		WaitI2CDone( I2C3_MASTER_BASE);

		/** Set read mode.  */
		I2CMasterSlaveAddrSet( I2C3_MASTER_BASE, 112, true);   // false = write, true = read

		I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
		vTaskDelay( 5 );
		WaitI2CDone( I2C3_MASTER_BASE);
		distance = I2CMasterDataGet(I2C3_MASTER_BASE) << 8;

		I2CMasterControl( I2C3_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		vTaskDelay( 5 );
		WaitI2CDone( I2C3_MASTER_BASE);
		distance |= I2CMasterDataGet(I2C3_MASTER_BASE);

		smoothed_distance = (smoothed_distance + distance) >> 1;

		//range of ultrasonic readings approx.
		//20cm to 600cm
		//above 600, treat as an error?
		if(smoothed_distance < 20)
			range = 20;
		else if(smoothed_distance > range_bound)
			range = range_bound;
		else
			range = smoothed_distance;

		//target = distance * 4050 >> 8;

		//UARTprintf("\x1b[2J\x1b[1;1HT=%d, D=%d, R=%d, B=%d\r", target, distance, range, range_bound );

	#elif RANGING_MODE == LASER_PWM
		//TODO: what is the counting frequency of WTIMER0? Is it 50MHz?
		//laser pulse width is 10 microseconds per cm
		//range should be in units of 1cm.

		//for ultrasonic sensor, range was 58 microseconds per cm

		//assuming clock increments at 50MHz?
		//range = echo_time / 500;

		//UARTprintf("\x1b[2J\x1b[1;1H%l cm\r", echo_time );

		//vTaskDelayUntil(&ulWakeTime, /*SONIC_TASK_DELAY*/ 100  );
		//laser
		//GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
		//vTaskDelayUntil(&ulWakeTime, configTICK_RATE_HZ >> 5 );
		//GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
		//vTaskDelayUntil(&ulWakeTime, configTICK_RATE_HZ >> 5 );
		if( laser_or_ultrasonic == 0 )
		{
			range = echo_time;
			//UARTprintf("\x1b[2J\x1b[1;1H%l counts (L)\r", echo_time );
		}
		else
		{
			//UARTprintf("\x1b[2J\x1b[1;1H%d cm\r", distance );
		}

		target = range * 4050 / range_bound;

	#elif RANGING_MODE == ULTRASONIC_PWM

	#else

	#endif
		vTaskDelayUntil(&ulWakeTime, /*SONIC_TASK_DELAY*/ 100  );
	}
}
