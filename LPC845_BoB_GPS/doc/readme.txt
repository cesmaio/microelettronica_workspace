	## Overview
 * 	-----------
 * 	This example reads and parses GPS data from the module when the user button is pressed, and outputs
 * 	the information to the console. The project has been configure to use the MCUXpresso IDE console.
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
