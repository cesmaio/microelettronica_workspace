/*
 * The Clear BSD License
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
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
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
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
 * 	The LPC845_BoB_CAPTocuh application shows how to interface with the on-board Cap touch button using SDK drivers.
 *  After an initial calibration, once the cap touch button is touched, the RGB red led will turn on.
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
 *
 *  ## Board Settings
 *	------------------------
 *	No special settings are required. The project is set to output messages to the MCUXpresso IDE Console
 *
*/

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_capt.h"
#include "fsl_power.h"
#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_CAPT_BASE CAPT
#define DEMO_CAPT_NOISE 0U
#define DEMO_CAPT_IRQ_NUMBER CMP_CAPT_IRQn
#define DEMO_CAPT_IRQ_HANDLER_FUNC CMP_CAPT_DriverIRQHandler
#define DEMO_CAPT_ENABLE_PINS kCAPT_X0Pin
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static uint32_t CAPT_DoCalibration(CAPT_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
* @brief ISR for CAPT (Yes o Not touch).
*/
void DEMO_CAPT_IRQ_HANDLER_FUNC(void)
{
    uint32_t mask;

    mask = CAPT_GetInterruptStatusFlags(DEMO_CAPT_BASE);
    CAPT_ClearInterruptStatusFlags(DEMO_CAPT_BASE, mask);
    if (mask & kCAPT_InterruptOfYesTouchEnable)
    {
    	GPIO_PortClear(GPIO, BOARD_INITPINS_RED_PORT, 1u << BOARD_INITPINS_RED_PIN);
	}
	else
	{
		GPIO_PortSet(GPIO, BOARD_INITPINS_RED_PORT, 1u << BOARD_INITPINS_RED_PIN);
	}
}

/*!
 * @brief Main function
 */
int main(void)
{
	uint32_t thresold;
	capt_config_t captConfig;

	/* Define the init structure for the output LED pin*/
	gpio_pin_config_t led_config = {
		kGPIO_DigitalOutput, 0,
	};

    /* Attach main clock to CAPT */
    CLOCK_Select(kCAPT_Clk_From_Fro);
    POWER_DisablePD(kPDRUNCFG_PD_ACMP);

    BOARD_InitPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    PRINTF("CAPT basic example.\r\n");
    PRINTF("\r\nKeep touch the Cap Touch button pressed and press Enter to start the Calibration\r\n");
    GETCHAR();

    /* Init output LED GPIO. */
	GPIO_PortInit(GPIO, BOARD_INITPINS_RED_PORT);
	GPIO_PinInit(GPIO, BOARD_INITPINS_RED_PORT, BOARD_INITPINS_RED_PIN, &led_config);
	GPIO_PortSet(GPIO, BOARD_INITPINS_RED_PORT, 1u << BOARD_INITPINS_RED_PIN);

    /* Initialize CAPT module. */
	CAPT_GetDefaultConfig(&captConfig);

	/* Calculate the clock divider to make sure CAPT work in 2Mhz fclk. */
	captConfig.clockDivider = (CLOCK_GetFroFreq() / 2000000U - 1U);
	captConfig.enableXpins = DEMO_CAPT_ENABLE_PINS;
	CAPT_Init(DEMO_CAPT_BASE, &captConfig);

	/* Do the self-calibration. */
	thresold = CAPT_DoCalibration(DEMO_CAPT_BASE);
	CAPT_SetThreshold(DEMO_CAPT_BASE, thresold - DEMO_CAPT_NOISE);
	PRINTF("\r\nCalibration has finished.\r\n");

	 /* Enable the interrupts. */
	CAPT_EnableInterrupts(DEMO_CAPT_BASE, kCAPT_InterruptOfYesTouchEnable | kCAPT_InterruptOfNoTouchEnable);
	NVIC_EnableIRQ(DEMO_CAPT_IRQ_NUMBER);

	/* Set polling mode and start poll. */
	CAPT_SetPollMode(DEMO_CAPT_BASE, kCAPT_PollContinuousMode);


    while (1)
    {
    }
}


/*!
* @brief Calibrate the CapTocuh button.
*/
static uint32_t CAPT_DoCalibration(CAPT_Type *base)
{
    uint16_t xpinIndex, repeatCounter;
    uint32_t temp_count, temp_count_high, temp_count_low;
    uint32_t temp_xPins, max_xpins;
    uint32_t average_count[16] = {0U};

    max_xpins = CAPT_GET_XMAX_NUMBER(CAPT_GetStatusFlags(base));
    temp_xPins = base->CTRL & CAPT_CTRL_XPINSEL_MASK;
    if (base->POLL_TCNT & CAPT_POLL_TCNT_TCHLOW_ER_MASK)
    {
        temp_count = 0xFFFU;
    }
    else
    {
        temp_count = 0U;
    }
    for (xpinIndex = 0U; xpinIndex <= max_xpins; xpinIndex++)
    {
        if (temp_xPins & (1U << (CAPT_CTRL_XPINSEL_SHIFT + xpinIndex)))
        {
            /* Before writing into CTRL register, INCHANGE(bit 15)should equal '0'. */
            while (CAPT_CTRL_INCHANGE_MASK == (CAPT_CTRL_INCHANGE_MASK & base->CTRL))
            {
            }
            base->CTRL = (base->CTRL & ~CAPT_CTRL_XPINSEL_MASK) | 1U << (CAPT_CTRL_XPINSEL_SHIFT + xpinIndex);
            for (repeatCounter = 0U; repeatCounter < 100U; ++repeatCounter)
            {
                /* Before writing into CTRL register, INCHANGE(bit 15)should equal '0'. */
                while (CAPT_CTRL_INCHANGE_MASK == (CAPT_CTRL_INCHANGE_MASK & base->CTRL))
                {
                }
                /* Start poll-now mode. */
                base->CTRL &= ~CAPT_CTRL_POLLMODE_MASK;
                base->CTRL |= CAPT_CTRL_POLLMODE(kCAPT_PollNowMode);
                /* Wait for poll-now done. */
                while (!(base->STATUS & CAPT_STATUS_POLLDONE_MASK))
                {
                }
                /* Clear the status flags. */
                base->STATUS |= base->STATUS;
                average_count[xpinIndex] += ((base->TOUCH & CAPT_TOUCH_COUNT_MASK) >> CAPT_TOUCH_COUNT_SHIFT);
            }
            average_count[xpinIndex] /= 100U;
        }
    }
    /* Restore the xpins. */
    base->CTRL = (base->CTRL & ~CAPT_CTRL_XPINSEL_MASK) | temp_xPins;

    temp_count_high = 0U;
    temp_count_low = 0xFFFU;
    for (xpinIndex = 0U; xpinIndex <= max_xpins; xpinIndex++)
    {
        if (temp_xPins & (1U << (CAPT_CTRL_XPINSEL_SHIFT + xpinIndex)))
        {
            if (temp_count_high < average_count[xpinIndex])
            {
                temp_count_high = average_count[xpinIndex];
            }
            if (temp_count_low > average_count[xpinIndex])
            {
                temp_count_low = average_count[xpinIndex];
            }
        }
    }
    /* For touch lower mode.*/
    if (base->POLL_TCNT & CAPT_POLL_TCNT_TCHLOW_ER_MASK)
    {
        temp_count = 2 * temp_count_low - temp_count_high;
    }
    else /* For touch higher mode. */
    {
        temp_count = 2 * temp_count_high - temp_count_low;
    }

    return temp_count;
}
