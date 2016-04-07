/*
 * @brief FreeRTOS blinky example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/
void led_set_rgb(uint32_t rgb);
/* Sets up system hardware */
static void prvSetupHardware(void)
{
	/* Generic Initialization */
	SystemCoreClockUpdate();
	Board_Init();
}

/* LED0 toggle thread */
static void vLEDTask0(void *pvParameters) {
	bool LedState = false;
	while (1) {
		Board_LED_Set(0, LedState);
		LedState = (bool) !LedState;

		vTaskDelay(configTICK_RATE_HZ / 2);
	}
}

/* LED1 toggle thread */
static void vLEDTask1(void *pvParameters) {
	bool LedState = false;
	while (1) {
		Board_LED_Set(1, LedState);
		LedState = (bool) !LedState;

		if (LedState)
		{
			led_set_rgb(0x900000);
		}
		else
		{
			led_set_rgb(0x009000);
		}

		vTaskDelay(configTICK_RATE_HZ * 2);
	}
}

/* LED2 toggle thread */
static void vLEDTask2(void *pvParameters) {
	bool LedState = false;
	while (1) {
		Board_LED_Set(2, LedState);
		LedState = (bool) !LedState;

		vTaskDelay(configTICK_RATE_HZ);
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	main routine for FreeRTOS blinky example
 * @return	Nothing, function should not exit
 */
#define SPEED_100KHZ         100000
#define SPEED_400KHZ         400000

#define I2C_SLAVE_LED_PWM_ADDR       0x62		// 1100010(R/W)

#define I2C_BUFF_SIZE	16
static uint8_t i2c_buff[I2C_BUFF_SIZE];

//static I2C_XFER_T seep_xfer;

/* State machine handler for I2C0 and I2C1 */
static void i2c_state_handling(I2C_ID_T id)
{
	if (Chip_I2C_IsMasterActive(id)) {
		Chip_I2C_MasterStateHandler(id);
	} else {
		Chip_I2C_SlaveStateHandler(id);
	}
}


void I2C_IRQHandler(void)
{
	i2c_state_handling(I2C0);
}

void led_set_rgb(uint32_t rgb)
{
	uint8_t data[2];

	data[0]=0x03; // LED1 on - red
	data[1] = (uint8_t)(0x000000FF & (rgb>>16));
	Chip_I2C_MasterSend(I2C0,I2C_SLAVE_LED_PWM_ADDR,data,2);

	data[0]=0x04;// LED2 on - green
	data[1] = (uint8_t)(0x000000FF & (rgb>>8));
	Chip_I2C_MasterSend(I2C0,I2C_SLAVE_LED_PWM_ADDR,data,2);

	data[0]=0x02;// LED0 on - blue
	data[1] = (uint8_t)(0x000000FF & (rgb));
	Chip_I2C_MasterSend(I2C0,I2C_SLAVE_LED_PWM_ADDR,data,2);
}

int main(void)
{
	int ret;
	volatile uint32_t delayCount;

	prvSetupHardware();

	Chip_I2CM_Init(LPC_I2C);
//	Chip_I2CM_SetBusSpeed(LPC_I2C, SPEED_100KHZ);

	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, SPEED_100KHZ);

	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandler);
	NVIC_EnableIRQ(I2C0_IRQn);

	memset(&i2c_buff[1], 0xFF, I2C_BUFF_SIZE);
	i2c_buff[0] = 0x08;
	i2c_buff[1] = 0x01; // LED0 on - blue
	Chip_I2C_MasterSend(I2C0,I2C_SLAVE_LED_PWM_ADDR,i2c_buff,2);

	memset(i2c_buff,0x5E,I2C_BUFF_SIZE);
	ret = Chip_I2C_MasterCmdRead(I2C0, (I2C_SLAVE_LED_PWM_ADDR), 0x00, i2c_buff, 1);	// Read Mode1 Register

	if (i2c_buff[0] & 0x10)
	{
		i2c_buff[1] = i2c_buff[0] & (~0x10); // Normal Mode
		i2c_buff[0]=0x00;
		Chip_I2C_MasterSend(I2C0,I2C_SLAVE_LED_PWM_ADDR,i2c_buff,2);
	}

	memset(i2c_buff,0x5E,I2C_BUFF_SIZE);
	ret = Chip_I2C_MasterCmdRead(I2C0, (I2C_SLAVE_LED_PWM_ADDR), 0x00, i2c_buff, 1);	// Read Mode1 Register

	memset(i2c_buff,0x5E,I2C_BUFF_SIZE);
	ret = Chip_I2C_MasterCmdRead(I2C0, (I2C_SLAVE_LED_PWM_ADDR), 0x08, i2c_buff, 1);	// Read LEDOUT Register

	i2c_buff[0]=0x08;
	i2c_buff[1]= 0x00;	// turn OFF all
	Chip_I2C_MasterSend(I2C0,I2C_SLAVE_LED_PWM_ADDR,i2c_buff,2);
	for (delayCount=0; delayCount< 1000000;delayCount++);

	i2c_buff[0]=0x08;
	i2c_buff[1]= 0x01; // LED0 on - blue
	Chip_I2C_MasterSend(I2C0,I2C_SLAVE_LED_PWM_ADDR,i2c_buff,2);
	for (delayCount=0; delayCount< 10000000;delayCount++);

	i2c_buff[0]=0x08;
	i2c_buff[1]= 0x04; // LED1 on - red
	Chip_I2C_MasterSend(I2C0,I2C_SLAVE_LED_PWM_ADDR,i2c_buff,2);
	for (delayCount=0; delayCount< 10000000;delayCount++);

	i2c_buff[0]=0x08;
	i2c_buff[1]= 0x10; // LED2 on - green
	Chip_I2C_MasterSend(I2C0,I2C_SLAVE_LED_PWM_ADDR,i2c_buff,2);
	for (delayCount=0; delayCount< 10000000;delayCount++);

	i2c_buff[0]=0x08;
	i2c_buff[1]= 0x40; // LED3 on - BL
	Chip_I2C_MasterSend(I2C0,I2C_SLAVE_LED_PWM_ADDR,i2c_buff,2);

	i2c_buff[0]=0x08;
	i2c_buff[1]= 0x2A; // LED3 on - PWM
	Chip_I2C_MasterSend(I2C0,I2C_SLAVE_LED_PWM_ADDR,i2c_buff,2);
	for (delayCount=0; delayCount< 10000000;delayCount++);

	/* LED1 toggle thread */
	xTaskCreate(vLEDTask1, (signed char *) "vTaskLed1",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* LED2 toggle thread */
	xTaskCreate(vLEDTask2, (signed char *) "vTaskLed2",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* LED0 toggle thread */
	xTaskCreate(vLEDTask0, (signed char *) "vTaskLed0",
				configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL),
				(xTaskHandle *) NULL);

	/* Start the scheduler */
	vTaskStartScheduler();

	/* Should never arrive here */
	return 1;
}

