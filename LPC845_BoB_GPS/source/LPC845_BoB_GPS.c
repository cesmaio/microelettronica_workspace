/*
 * Copyright 2016-2018 NXP
 * All rights reserved.
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
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
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
 * 	This example reads and parses GPS data from the module when the user button is pressed, and outputs
 * 	the information to the terminal program.
 *
 *	## Toolchain Supported
 *	---------------------
 *	- MCUXpresso IDE v10.3
 *
 *	## Hardware Requirements
 *	------------------------
 *	- Micro USB cable
 *	- LPC845 Breakout board
 *	- Personal Computer
 *	- Adafruit Flora wearable ultimate GPS module (FW5632)
 *	  https://www.adafruit.com/product/746
 *
 *  ## Board Settings
 *	Connect pins of USART for the GPS as below:
 *	LPC845        CONNECTS TO      	    GPS module
 *	Pin Name      Board Location     	Pin Name
 *	USART1_TXD    PIO0_23            	GPS_RX
 *	USART1_RXD	  PIO0_22			    GPS_TX
 *	VCC		      CN1-40              	GPS_GND
 *	GND           CN1-20              	GPS_3.3v
 *
*/

/**
 * @file    LPC845_Project_GPS.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC845.h"
#include "fsl_debug_console.h"
#include "fsl_GPSModule.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_SW_PORT 0U
#define APP_SW_PIN 4U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void delay(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief delay a while.
 */
void delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 200000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

int main(void) {


	GPS_location_data_t LocationData;
	uint32_t port_state = 0;

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();

  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    /* Select the main clock as source clock of USART1. */
    CLOCK_Select(kUART1_Clk_From_MainClk);

    /*Init SW Port*/
    GPIO_PortInit(GPIO, APP_SW_PORT);

    /* Initialize USART with configuration. */
	GPS_Init();

	/*Only received GPRMC and GGA Data*/
	GPS_SendCommand((uint8_t *)PMTK_SET_NMEA_OUTPUT_RMCGGA);

	/*1 Hz update rate*/
	GPS_SendCommand((uint8_t *)PMTK_SET_NMEA_UPDATE_5HZ);

	/*Welcome message*/
	PRINTF("\r\nThis is a GPS module example, the location data will be shown below\n\r");

    while(1)
    {
    	 GPS_ReceivedData();
 		 GPS_Parse(&LocationData);
    	 port_state = GPIO_PortRead(GPIO, 0);
		 if (!(port_state & (1 << APP_SW_PIN)))
		 {
    		PRINTF("\r\nTime: %d:%d:%d\r\n",LocationData.Hour,LocationData.Minute,LocationData.Seg);
			PRINTF("\rDate: %d/%d/%d\r\n",LocationData.Day,LocationData.Month,LocationData.Year);
			PRINTF("\rFix: %d\r\n",LocationData.Fix);
			PRINTF("\rQuality: %d\r\n", LocationData.Quality);
			PRINTF("\rLocation: %d %d.%d%c', %d %d.%d%c'\r\n",LocationData.LatitudDegree,LocationData.LatitudMinute,LocationData.LatitudSeg,LocationData.LatitudCompass,LocationData.LongitudDegree,LocationData.LongitudMinute,LocationData.LongitudSeg,LocationData.LongitudCompass);
			PRINTF("\rSpeed: %d.%d\r\n", LocationData.SpeedI,LocationData.SpeedF);
			PRINTF("\rAngle: %d.%d\r\n", LocationData.AngleI,LocationData.AngleF);
			PRINTF("\rAltitude: %d\r\n", LocationData.Altitude);
			PRINTF("\rSatelites: %d\r\n", LocationData.Satellites);
			delay();
    	}

    }
    return 0 ;
}
