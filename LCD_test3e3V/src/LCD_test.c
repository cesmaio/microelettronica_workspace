
/*
===============================================================================
 Name        : LCD_test.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "LPC8xx.h"
#include "HD44780.h"
#include "Delay.h"
#include "LED.h"

int main(void) {

    LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<20;   // Turn on clock to GPIO1
    ACT_RGB

    InitDelay(); //inizializza ch0 di MRT per funzioni delay
	InitLCD(); //initialize LCD

	while(1) {
		WriteLine((uint8_t*)"LP"); // scrive una stringa sulla riga sopra (16 caratteri)
		WriteChar('C');                 // scrive un carattere
		WriteChar(NUM_TO_CODE(8));      // scrive il carattere ASCII relativo a un numero
		Write_2digitsval(45);           // scrive un valore decimale a due cifre
		WriteChar(0x40);				// cuoricino
		DelayMs(1000);					// pause

		WriteVisible((uint8_t*)"***test LCD 1602"); // scrive una stringa dall'inizio
		DelayMs(1000);		// pause
		WriteLine((uint8_t*)"abcdefghijklmnopqrstuvwxyz123456"); // se la stringa è più lunga, la scrive dopo
		DelayMs(1000);		// pause
		DisplayLeft();
		DelayMs(1000);		// pause

		PutCommand(RETURN_HOME); DelayMs(5);
		WriteLine((uint8_t*)"**** LPC");  // WriteString comincia a scrivere a sinistra
		WriteAfter((uint8_t*)"845 ****");       // WriteWord continua a scrivere dalla posizione del cursore
		DelayMs(1000);		// pause

		GoToLine(1);  DelayUs(100);
		Write_ndigitsval( 12345678, 8);  // WriteString comincia a scrivere a sinistra
		WriteAfter((uint8_t*)"        ");       // WriteWord continua a scrivere dalla posizione del cursore
		DelayMs(1000);		// pause

		LED_R //on
		DelayMs(500);
		R_OFF
		DelayMs(500);

		PutCommand(DISPLAY_CLEAR); DelayMs(5);
		DelayMs(1000);		// pause
  } // end of while(1)
	
} // end of main
