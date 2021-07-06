/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*	## Overview
 * 	-----------
 * 	The accel_led application shows how to interface off-shelf components using SDK drivers for I2C.
 * 	In this example the LPC845 Breakout board is interfaced with a MMA8652 accelerometer breakout board.
 * 	The accelerometer acts as an I2C slave device to the LPC845 master device.
 * 	Colors on the on-board RGB LED should change when accelerometer position is changed.
 *
 * 	By default, the I2C timeout is disabled, so the code will continuously attempt to read I2C. A timeout
 * 	can be enabled by defining I2C_WAIT_TIMEOUT in fsl_i2c.h with a (fairly large) non-zero value.
 *
 *	## Toolchain Supported
 *	---------------------
 *	- MCUXpresso IDE v10.3
 *
 *	## Hardware Requirements
 *	------------------------
 *	- Micro USB cable
 *	- LPC845 Breakout board
 *	- MMA8652 break-out board (BRKT-STBC-AGM04) from NXP and its distributors
 *	- External 2.2Kohm pull-up resistors (on SCL and SDA lines)
 *	- Personal Computer
 *
 *	## Board Settings
 *	------------------------
 *	Connect pins of I2C master and slave as below:
 *	MASTER_BOARD        CONNECTS TO         SLAVE_BOARD
 *	Pin Name   Board Location     Pin Name   Board Location
 *	SCL        CN1-23              SCL        J2-3
 *	SDA        CN1-22              SDA        J2-4
 *	VCC		   CN1-40              VDD/VDDIO  J1-3/4
 *	GND        CN1-20              GND        J1-5
 *
 */

/*******************************************************************************
 * Standard C Included Files
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "board.h"
#include "fsl_mma.h"
#include "fsl_i2c.h"
#include "fsl_debug_console.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* LED port and pins */
#define BOARD_LED0_PORT BOARD_INITPINS_GREEN_PORT
#define BOARD_LED0_PIN BOARD_INITPINS_GREEN_PIN
#define BOARD_LED1_PORT BOARD_INITPINS_BLUE_PORT
#define BOARD_LED1_PIN BOARD_INITPINS_BLUE_PIN
#define BOARD_LED2_PORT BOARD_INITPINS_RED_PORT
#define BOARD_LED2_PIN BOARD_INITPINS_RED_PIN

#define EXAMPLE_I2C_MASTER_BASE (I2C0_BASE)
#define I2C_MASTER_CLOCK_FREQUENCY (12000000)
#define EXAMPLE_I2C_MASTER ((I2C_Type *)EXAMPLE_I2C_MASTER_BASE)
#define I2C_MASTER_SLAVE_ADDR_7BIT (0x7EU)
#define I2C_BAUDRATE (100000) /* 100K */
#define I2C_DATA_LENGTH (33)  /* MAX is 256 */

/*******************************************************************************
 * Variables
 ******************************************************************************/
i2c_master_handle_t g_MasterHandle;
i2c_master_config_t masterConfig;

mma_handle_t mmaHandle = {0};
mma_data_t accelSensorData = {0};

/* Define the init structure for the output LED pin*/
gpio_pin_config_t led_config =
{
	kGPIO_DigitalOutput, 0,
};

status_t status = kStatus_Fail;
volatile bool g_MasterCompletionFlag = false;

/* MMA8652 device address */
const uint8_t g_accel_address = 0x1DU;
uint8_t accelGValue = 0;
uint8_t dataScale = 0;
int16_t accelXData = 0;
int16_t accelYData = 0;
int16_t accelZData = 0;
int16_t xAngle = 0;
int16_t yAngle = 0;
int16_t zAngle = 0;

/*******************************************************************************
* I2C Accelerometer Interface Functions
******************************************************************************/
void i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData)
{
    /* Signal transfer success when received success status. */
    if (status == kStatus_Success)
    {
        g_MasterCompletionFlag = true;
    }
}

void init_i2c_master()
{
	mmaHandle.base = BOARD_ACCEL_I2C_BASEADDR;
	masterConfig.baudRate_Bps = I2C_BAUDRATE;
	mmaHandle.i2cHandle = &g_MasterHandle;
	mmaHandle.xfer.slaveAddress = g_accel_address;
	mmaHandle.xfer.direction = kI2C_Write;

    masterConfig.enableTimeout = true;

	/* Initialize and set default configuration: clock, freq for I2C master device */
	I2C_MasterGetDefaultConfig(&masterConfig);
	I2C_MasterInit(BOARD_ACCEL_I2C_BASEADDR, &masterConfig, I2C_MASTER_CLOCK_FREQUENCY);
	I2C_MasterTransferCreateHandle(BOARD_ACCEL_I2C_BASEADDR, &g_MasterHandle, i2c_master_callback, NULL);
}

/*******************************************************************************
 * Accelerometer Initialization function
 ******************************************************************************/
int init_accel(void)
{
	/* Initialize the accelerometer device:- Dynamic range : -4g to +4g; FIFO : disabled; F_READ : Normal Read mode */
	if (MMA_Init(&mmaHandle) != kStatus_Success)
	{
		PRINTF("\n Failed MMA_Init\n\r");
		return kStatus_Fail;
	}

	/* Get Accelerometer dynamic range */
	if (MMA_ReadReg(&mmaHandle, kMMA8652_XYZ_DATA_CFG, &accelGValue) != kStatus_Success)
	{
		PRINTF("\n Failed MMA_ReadReg");
		return kStatus_Fail;
	}

	return kStatus_Success ;
}


/*******************************************************************************
 * Accelerometer value reading function
 ******************************************************************************/
int accel_read_value(void)
{
	if (MMA_ReadSensorData(&mmaHandle, &accelSensorData) != kStatus_Success)
	{
		PRINTF("\n\r Read data failed");
		return kStatus_Fail;
	}

	return kStatus_Success ;
}

/*******************************************************************************
 * Main function
 ******************************************************************************/
int main(void)
{
    /* Initialize Board pins, clock and debug console unit*/
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* I2C master-slave device initialization */
    init_i2c_master();
    status = init_accel();

    if (status == kStatus_Fail)
    {
    	PRINTF("Could not initialize accelerometer\n\r");
    	return -1;
    }

    /*Check FS bits[1:0] for dynamic range value : 2g = 0x00; 4g = 0x01; 8g = 0x02 */
    switch (accelGValue & 0x03)
	{
		case 0x00: dataScale = 2;
				   break;
		case 0x01: dataScale = 4;
				   break;
		case 0x02: dataScale = 8;
				   break;
	}

    /* Init output LED GPIO Port */
    GPIO_PortInit(GPIO, BOARD_LED0_PORT);
    GPIO_PortInit(GPIO, BOARD_LED1_PORT);
    GPIO_PortInit(GPIO, BOARD_LED2_PORT);

    /* Init output LED GPIO Pin */
    GPIO_PinInit(GPIO, BOARD_LED0_PORT, BOARD_LED0_PIN, &led_config);
    GPIO_PinInit(GPIO, BOARD_LED1_PORT, BOARD_LED1_PIN, &led_config);
    GPIO_PinInit(GPIO, BOARD_LED2_PORT, BOARD_LED2_PIN, &led_config);

    while(1)
	{
		/* Read Accelerometer Sensor Data*/
		status = accel_read_value();

		/* Save each sensor data as a 16 bit result.
		* The 16 bit result is in left-justified format.
		* Shift 4 bits to the right(12-bit resolution) to get the actual value.
		*/
		accelXData =
			(int16_t)(((uint16_t)(accelSensorData.accelXMSB << 8)) | ((uint16_t)(accelSensorData.accelXLSB))) / 16U;
		accelYData =
			(int16_t)(((uint16_t)(accelSensorData.accelYMSB << 8)) | ((uint16_t)(accelSensorData.accelYLSB))) / 16U;
		accelZData =
			(int16_t)(((uint16_t)(accelSensorData.accelZMSB << 8)) | ((uint16_t)(accelSensorData.accelZLSB))) / 16U;

		/* Convert raw accelerometer sensor data to angle (normalize to 0-90 degrees). No negative angles. */
		xAngle = (int16_t)floor((double)accelXData * (double)dataScale * 180 / 4096);
		yAngle = (int16_t)floor((double)accelYData * (double)dataScale * 180 / 4096);

		PRINTF("\n\t X angle = %d Y angle = %d",xAngle,yAngle);

	   /* Change the color of the on-board LED corresponding to the y-angle value.
		* When the accelerometer is rotated along the y-axis
		* (facing the J1 connector on accelerometer board),
		*  at extreme left:	 	Blue color LED turns ON ,
		*  at extreme right: 	Green color LED turns ON,
		*  at the center:	 	Red color LED turns ON,
		*/
		if((yAngle < 90) && (yAngle > 30))
		{
			/* Turn Blue LED ON */
			GPIO_PinWrite(GPIO, BOARD_LED0_PORT,BOARD_LED0_PIN,1);
			GPIO_PinWrite(GPIO, BOARD_LED1_PORT,BOARD_LED1_PIN,0);
			GPIO_PinWrite(GPIO, BOARD_LED2_PORT,BOARD_LED2_PIN,1);
		}

		if((yAngle < 30) && (yAngle > -30))
		{
			/* Turn Green LED ON */
			GPIO_PinWrite(GPIO, BOARD_LED0_PORT,BOARD_LED0_PIN,0);
			GPIO_PinWrite(GPIO, BOARD_LED1_PORT,BOARD_LED1_PIN,1);
			GPIO_PinWrite(GPIO, BOARD_LED2_PORT,BOARD_LED2_PIN,1);
		}

		if((yAngle < -30) && (yAngle > -90))
		{
			/* Turn Red LED ON */
			GPIO_PinWrite(GPIO, BOARD_LED0_PORT,BOARD_LED0_PIN,1);
			GPIO_PinWrite(GPIO, BOARD_LED1_PORT,BOARD_LED1_PIN,1);
			GPIO_PinWrite(GPIO, BOARD_LED2_PORT,BOARD_LED2_PIN,0);
		}

	 }
}
