/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
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

/*
 * ## Overview
-----------
The SCTimer project is a simple demonstration program of the SDK SCTimer's driver capability to generate PWM signals.
The PWM output can be observed with an oscilloscope and by observing the green LED on the board.

## Functional Description
-------------------------
In this example, The SCTimer module generates two PWM signals.

The example will initialize clock, pin mux configuration, , then configure the SCTimer module to make it work.
The example configures first 24kHz PWM with (an initial) 60% duty cycle from SCTIMER output 2. Keyboard presses
(from the console) can then be used to trigger updates to the PWM.

## Toolchain Supported
---------------------
- MCUXpresso 10.3

## Hardware Requirements
------------------------
- Micro USB cable
- LPC845 Breakout board
- Personal Computer
- Oscilloscope to observe PWM output (optional)

## Board Settings
------------------------
No special settings are required.

## Expected Result
------------------------
The PWM output can be observed at the PIO1_0 port using an oscilloscope, and also by watching the green LED.
Since the LED is on when the PWM output is low, the higher the duty cycle, the dimmer the LED.
 */


#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_sctimer.h"
#include "fsl_swm_connections.h"
#include "pin_mux.h"
#include <stdbool.h>
/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define SCTIMER_CLK_FREQ CLOCK_GetFreq(kCLOCK_Fro)
#define DEMO_FIRST_SCTIMER_OUT kSCTIMER_Out_2

#define BOARD_LED_PORT BOARD_INITPINS_Green_LED_PORT //port 1 pin 0
#define BOARD_LED_PIN BOARD_INITPINS_Green_LED_PIN
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint32_t g_systickCounter;
/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
#define INITIAL_DUTY_CYCLE 60U

int main(void)
{
    sctimer_config_t sctimerInfo;
    sctimer_pwm_signal_param_t pwmParam;
    uint32_t event;
    uint32_t sctimerClock;
    int up_down = 1;
    uint8_t dutyCycle = INITIAL_DUTY_CYCLE;

    /* Board pin, clock, debug console init */
    /* Attach 12 MHz clock to USART0 (debug console) */
    CLOCK_Select(BOARD_DEBUG_USART_CLK_ATTACH);

    BOARD_InitPins();
    BOARD_BootClockFRO30M();
    BOARD_InitDebugConsole();

    sctimerClock = SCTIMER_CLK_FREQ;

    /* Print a note to terminal */
    PRINTF("SCTimer example to output center-aligned PWM signals\r\n");
    PRINTF("The PWM signal can be observed at using an oscilloscope\r\n");
    PRINTF("probing the PIO1_0 pin, which also drives the green LED.\r\n");

    SCTIMER_GetDefaultConfig(&sctimerInfo);

    /* Initialize SCTimer module */
    SCTIMER_Init(SCT0, &sctimerInfo);

    /* Configure first PWM with frequency 24kHZ from first output */
    pwmParam.output = DEMO_FIRST_SCTIMER_OUT;
    pwmParam.level = kSCTIMER_HighTrue;
    pwmParam.dutyCyclePercent = dutyCycle;

    if (SCTIMER_SetupPwm(SCT0, &pwmParam, kSCTIMER_CenterAlignedPwm, 24000U, sctimerClock, &event) == kStatus_Fail)
    {
    	PRINTF ("Could not start SC Timer\n\r");
    	return -1;
    }

    /* Start the timer */
    SCTIMER_StartTimer(SCT0, kSCTIMER_Counter_L);

    /* Endless loop, increasing / decreasing duty cycle 1% at a time after each key press from console */
    PRINTF ("Press Enter to change duty cycle. This will increase until if reaches 100, then count back down to 1.\n\r");
    while (1)
    {
     	GETCHAR();
     	dutyCycle += up_down;
    	PRINTF("Duty cycle %d%c\n\r", dutyCycle, 37);
    	if ((dutyCycle > 99) || (dutyCycle == 1)) up_down = -up_down;
    	SCTIMER_UpdatePwmDutycycle(SCT0, DEMO_FIRST_SCTIMER_OUT, dutyCycle, event);
   }
}
