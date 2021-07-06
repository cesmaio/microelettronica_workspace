/*
 * Copyright 2016-2018 NXP Semiconductor, Inc.
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
 * 	The LPC845_BoB_OlED application shows how to interface off-shelf components using SDK drivers for SPI.
 * 	In this example the LPC845 Breakout Board is interfaced with a OLED display.
 * 	The project has the possibility to print a string or to draw a image.
 * 	You are going to see the NXP logo and the name of the board in the display.
 *
 *	## Toolchain Supported
 *	---------------------
 *	- MCUXpresso IDE v10.3
 *
 *	## Hardware Requirements
 *	------------------------
 *	- Micro USB cable
 *	- LPC845 Breakout board
 *	- Adafruit OLED SSD1306
 *	- Personal Computer
 *
 *	Pins used in this application:
 *
 *	 LPC845    Description
 *	 -----     -----------
 *	 P0.26     OLED SCK
 *	 P0.21     OLED Data/MOSI
 *	 P0.20     OLED CS
 *	 P0.16     OLED Reset
 *	 P0.1      OLED Data/Command Select
 *
 *	 GND and 3.3V also needed for the OLED display.
 *	 */

/**
 * @file    LPC845_Project_OLED.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC845.h"
#include "fsl_debug_console.h"
#include "fsl_Systick_Delay.h"
#include "fsl_SSD1306.h"
#include "fsl_spi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define OLED_SPI_MASTER SPI1
#define OLED_CLK_SRC  kCLOCK_MainClk
#define OLED_SPI_MASTER_CLK_FREQ CLOCK_GetFreq(OLED_CLK_SRC)
#define OLED_SPI_MASTER_BAUDRATE 1000000U
#define OLED_SPI_MASTER_SSEL kSPI_Ssel0Assert

/*******************************************************************************
 * Code
 ******************************************************************************/

int main(void) {

	/* Define the init structure for the output DC pin*/
	gpio_pin_config_t led_DC_config = {
		kGPIO_DigitalOutput, 0,
	};

	/*Define the init structure for the output CS/RST pin*/
	gpio_pin_config_t CS_RST_config = {
		kGPIO_DigitalOutput, 1,
	};

	/*Define the init structure for the SPI*/
	spi_master_config_t masterConfig = {0};
	uint32_t srcFreq = 0U;

    /*Initialize the GPIO pins in to their initial state*/
	GPIO_PortInit(BOARD_OLED_CS_GPIO, BOARD_OLED_CS_GPIO_PORT);
	GPIO_PinInit(BOARD_OLED_DC_GPIO,BOARD_OLED_DC_GPIO_PORT,BOARD_OLED_DC_GPIO_PIN, &led_DC_config);
	GPIO_PinInit(BOARD_OLED_RST_GPIO,BOARD_OLED_RST_GPIO_PORT,BOARD_OLED_RST_GPIO_PIN, &CS_RST_config);
	GPIO_PinInit(BOARD_OLED_CS_GPIO,BOARD_OLED_CS_GPIO_PORT,BOARD_OLED_CS_GPIO_PIN, &CS_RST_config);

  	/* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
  	/* Init FSL debug console. */
    BOARD_InitDebugConsole();

    /* Attach main clock to SPI0. */
    CLOCK_Select(kSPI1_Clk_From_MainClk);

    /* Initialize the delay timer (SysTick) with 1ms intervals */
    Systick_Init(SystemCoreClock / 1000);

    /*Initialize SPI Master*/
    SPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = OLED_SPI_MASTER_BAUDRATE;
    masterConfig.sselNumber = OLED_SPI_MASTER_SSEL;
    masterConfig.clockPhase = kSPI_ClockPhaseFirstEdge;
    srcFreq = OLED_SPI_MASTER_CLK_FREQ;
    SPI_MasterInit(OLED_SPI_MASTER, &masterConfig, srcFreq);
    //SPI_WriteConfigFlags(OLED_SPI_MASTER,SPI_TXCTL_EOT_MASK | SPI_TXCTL_EOF_MASK | SPI_TXCTL_RXIGNORE_MASK);

    /* Initialize the SSD1306 display*/
    OLED_Init();
    OLED_Refresh();

    /*Print welcome message*/
    OLED_Copy_Image(&logo_nxp[0], sizeof(logo_nxp));
    OLED_Refresh();

    OLED_Set_Text(10, 52, kOLED_Pixel_Set, "LPC845 BoB", 2);
    OLED_Refresh();

    while(1) {

    }
    return 0 ;
}
