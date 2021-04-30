${ProjName}:
Blinky


Intended purpose:
To evaluate the IDE-hw functionality.


Functional description:
* The currently referenced LED is lit.
* A for cycle is used to create a delay.
* The LED is turned off.
* A for cycle is used to create a delay.
* The process repeats.


External connections:
* None


Detailed Program Flow:
* This example runs at 30 MHz.
  See function SystemInit() and chip_setup.h.


* main() routine
   1. Turn on clocks to peripherals
   2. Enter the main while(1) loop
      A. Drive the LED as outlined above

  
This example runs from Flash. 


To run this code: 1. Build
                  2. Load to flash
                  3. Reset
              or:
                  1. Build
                  2. Download and Debug
                  3. Run or Go


Note: It may be necessary to power-cycle the board in order to regain control of the reset button after programming.





