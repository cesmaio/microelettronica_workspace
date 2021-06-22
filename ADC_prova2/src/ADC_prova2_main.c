
//===============================================================================
// Name        : ADC_prova_main.c
// Author      : SSalvatori
// Version     :
// Copyright   : ---
// Description : Leggere LEGGIMI.txt
//===============================================================================

#include "LPC8xx.h"
#include "HD44780.h"
#include "timer.h"
#include "adc.h"

int main(void) {
  uint32_t dummyVal;

  // 0. Abilita clock per CTIMER0 (funzioni delay) e inizializza LCD
    LPC_SYSCON->SYSAHBCLKCTRL0 |= (1<<25);
    InitLCD_4b_2L(); //inizializza LCD

    InitADC();

  // Main loop
  while(1) {
	  PutCommand(LINE1_HOME);
	  for (uint8_t i=0; i<4; i++)
	  {dummyVal = ReadADC();
	  Write_ndigitsval(dummyVal, 4);
	  DelayMs(5000);}

	  PutCommand(LINE2_HOME);
	  for (uint8_t i=0; i<4; i++)
	 	  {dummyVal = ReadADC();
	 	  Write_ndigitsval(dummyVal, 4);
	 	  DelayMs(5000);}

  } // end of while 1
} // end of main

