 * ## Overview
-----------
The SCTimer project is a simple demonstration program of the SDK SCTimer's driver capability to generate PWM signals on the LPc845 Breakout board.
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
