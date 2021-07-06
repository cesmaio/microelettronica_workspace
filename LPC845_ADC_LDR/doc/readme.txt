/ 	## Overview
 	-----------
 	The adc_ldr_main application shows how to interface off-shelf components using SDK drivers for ADC.
 	In this example the LPC845 Breakout Board is interfaced with a LDR (Light Dependent Resistor)/Photo-resistor/Photo-cell.
 	The LDR is connected to the ADC channel-8 of the board through a resistor divider circuit.
 	Resistance values displayed on the console window should change when the light intensity changes.
  
  	## Toolchain Supported
  	---------------------
  	- MCUXpresso IDE v10.3
  - Should also be portable to Keil, IAR and other IDEs
  
  	## Hardware Requirements
  	------------------------
  	- Micro USB cable
  	- LPC845 Breakout board
  	- LDR/Photo-resistor
  	- 4.4Kohm resistor
  
  	## Board Settings
  	------------------------
  	Connect pins of ADC channel and LDR as below:
  	LPC845        		CONNECTS TO     LDR Resistor-divider ckt
  	Pin Name   Board Location     		Pin Name
  	ADC_8      PIO0_18             		resistor divider o/p
  	VCC		   CN1-40              		LDR
  	GND        CN1-20              		4.4k resistor
 
## Run the Project
------------------------
The program will continuously output the ADC values read. The default project settings are to output to the IDE console.

