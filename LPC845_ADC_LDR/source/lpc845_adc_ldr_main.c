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

/*	## Overview
 * 	-----------
 * 	The adc_ldr_main application shows how to interface off-shelf components using SDK drivers for ADC.
 * 	In this example the LPC845 Breakout Board is interfaced with a LDR (Light Dependent Resistor)/Photo-resistor/Photo-cell.
 * 	The LDR is connected to the ADC channel-8 of the LPC45 board through a resistor divider circuit.
 * 	Resistance values displayed on a terminal program should change when the light intensity is changed.
 *
 *	## Toolchain Supported
 *	---------------------
 *	- MCUXpresso IDE v10.3
 *  - Should also be portable to Keil, IAR and other IDEs
 *
 *	## Hardware Requirements
 *	------------------------
 *	- Micro USB cable
 *	- LPC845 Breakout board
 *	- LDR/Photo-resistor
 *	- 4.4Kohm resistor
 *
 *	## Board Settings
 *	------------------------
 *	Connect pins of ADC channel and LDR as below:
 *	LPC845        		CONNECTS TO     LDR Resistor-divider ckt
 *	Pin Name   Board Location     		Pin Name
 *	ADC_8      PIO0_18             		resistor divider o/p
 *	VCC		   CN1-40              		LDR
 *	GND        CN1-20              		4.4k resistor
 *
 */

/*******************************************************************************
 * Standard C Included Files
 ******************************************************************************/
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_adc.h"
#include "fsl_clock.h"
#include "fsl_power.h"
#include "pin_mux.h"
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_ADC_BASE BOARD_INITPINS_ADC_8_LDR_PERIPHERAL
#define DEMO_ADC_SAMPLE_CHANNEL_NUMBER BOARD_INITPINS_ADC_8_LDR_CHANNEL
#define DEMO_ADC_CLOCK_SOURCE kCLOCK_Fro

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void ADC_Configuration(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
adc_result_info_t adcResultInfoStruct;

/*******************************************************************************
 * Main function
 ******************************************************************************/
int main(void)
{
    /* Initialize board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    
    /* Configure the converter and work mode. */
    ADC_Configuration();

    while(1)
    {
        /* Trigger the sequence's conversion by software */
    	ADC_DoSoftwareTriggerConvSeqA(DEMO_ADC_BASE);

        /* Wait for the converter to be done. */
        while (!ADC_GetChannelConversionResult(DEMO_ADC_BASE, DEMO_ADC_SAMPLE_CHANNEL_NUMBER, &adcResultInfoStruct))
        {
        }

        /* Display the conversion result */
        PRINTF("adcResult      = %d\r\n", adcResultInfoStruct.result);
    }
}

/*******************************************************************************
 * ADC Configuration function
 ******************************************************************************/
static void ADC_Configuration(void)
{
	adc_config_t adcConfigStruct;
    adc_conv_seq_config_t adcConvSeqConfigStruct;

	#if !(defined(FSL_FEATURE_ADC_HAS_NO_CALIB_FUNC) && FSL_FEATURE_ADC_HAS_NO_CALIB_FUNC)
	#if defined(FSL_FEATURE_ADC_HAS_CALIB_REG) && FSL_FEATURE_ADC_HAS_CALIB_REG
		/* Calibration after power up. */
		if (ADC_DoSelfCalibration(DEMO_ADC_BASE))
	#else
		uint32_t frequency;
	#if defined(SYSCON_ADCCLKDIV_DIV_MASK)
		frequency = CLOCK_GetFreq(DEMO_ADC_CLOCK_SOURCE) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
	#else
		frequency = CLOCK_GetFreq(DEMO_ADC_CLOCK_SOURCE);
	#endif /* SYSCON_ADCCLKDIV_DIV_MASK */
		/* Calibration after power up. */
		if (ADC_DoSelfCalibration(DEMO_ADC_BASE, frequency))
	#endif /* FSL_FEATURE_ADC_HAS_CALIB_REG */
		{
			PRINTF("ADC_DoSelfCalibration() Done.\r\n");
		}
		else
		{
			PRINTF("ADC_DoSelfCalibration() Failed.\r\n");
		}
	#endif /* FSL_FEATURE_ADC_HAS_NO_CALIB_FUNC */


	/* Configure the converter. */
	#if defined(FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE) & FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE
		adcConfigStruct.clockMode = kADC_ClockSynchronousMode; /* Using sync clock source. */
	#endif                                                     /* FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE */
		adcConfigStruct.clockDividerNumber = 1;                /* The divider for sync clock is 2. */
	#if defined(FSL_FEATURE_ADC_HAS_CTRL_RESOL) & FSL_FEATURE_ADC_HAS_CTRL_RESOL
		adcConfigStruct.resolution = kADC_Resolution12bit;
	#endif /* FSL_FEATURE_ADC_HAS_CTRL_RESOL */
	#if defined(FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL) & FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL
		adcConfigStruct.enableBypassCalibration = false;
	#endif /* FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL */
	#if defined(FSL_FEATURE_ADC_HAS_CTRL_TSAMP) & FSL_FEATURE_ADC_HAS_CTRL_TSAMP
		adcConfigStruct.sampleTimeNumber = 0U;
	#endif /* FSL_FEATURE_ADC_HAS_CTRL_TSAMP */
	#if defined(FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE) & FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE
		adcConfigStruct.enableLowPowerMode = false;
	#endif /* FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE */
	#if defined(FSL_FEATURE_ADC_HAS_TRIM_REG) & FSL_FEATURE_ADC_HAS_TRIM_REG
		adcConfigStruct.voltageRange = kADC_HighVoltageRange;
	#endif /* FSL_FEATURE_ADC_HAS_TRIM_REG */
		ADC_Init(DEMO_ADC_BASE, &adcConfigStruct);

	#if !(defined(FSL_FEATURE_ADC_HAS_NO_INSEL) && FSL_FEATURE_ADC_HAS_NO_INSEL)
		/* Use the temperature sensor input to channel 0. */
		ADC_EnableTemperatureSensor(DEMO_ADC_BASE, true);
	#endif /* FSL_FEATURE_ADC_HAS_NO_INSEL. */

    /* Enable channel DEMO_ADC_SAMPLE_CHANNEL_NUMBER's conversion in Sequence A. */
    adcConvSeqConfigStruct.channelMask =
        (1U << DEMO_ADC_SAMPLE_CHANNEL_NUMBER); /* Includes channel DEMO_ADC_SAMPLE_CHANNEL_NUMBER. */
    adcConvSeqConfigStruct.triggerMask = 0U;
    adcConvSeqConfigStruct.triggerPolarity = kADC_TriggerPolarityPositiveEdge;
    adcConvSeqConfigStruct.enableSingleStep = false;
    adcConvSeqConfigStruct.enableSyncBypass = false;
    adcConvSeqConfigStruct.interruptMode = kADC_InterruptForEachSequence;
    ADC_SetConvSeqAConfig(DEMO_ADC_BASE, &adcConvSeqConfigStruct);
    ADC_EnableConvSeqA(DEMO_ADC_BASE, true); /* Enable the conversion sequence A. */
    /* Clear the result register. */
    ADC_DoSoftwareTriggerConvSeqA(DEMO_ADC_BASE);
    while (!ADC_GetChannelConversionResult(DEMO_ADC_BASE, DEMO_ADC_SAMPLE_CHANNEL_NUMBER, &adcResultInfoStruct))
    {
    }
    ADC_GetConvSeqAGlobalConversionResult(DEMO_ADC_BASE, &adcResultInfoStruct);
}
