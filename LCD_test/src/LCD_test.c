
/*
===============================================================================
 Name        : LCD_test.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include "HD44780.h"
#include "LED.h"

int main(void) {

	LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<20;   // Turn on clock to GPIO1
    ACT_RGB
	InitLCD_4b_2L(); //initialize LCD

	while(1) {
		WriteInitial((uint8_t*)"LP"); // scrive una stringa sulla riga sopra (16 caratteri)
		WriteAscii('C');                 // scrive un carattere
		WriteAscii(NUM_TO_CODE(8));      // scrive il carattere ASCII relativo a un numero
		Write_2digitsval(45);           // scrive un valore decimale a due cifre
		WriteAscii(0xF6);				// Sommatoria
		DelayMs(1000);					// pause

		WriteInitial((uint8_t*)"***test LCD 1602**"); // scrive una stringa dall'inizio
		DelayMs(1000);		// pause
		DisplayLeft(LCD_LINE_VISIBLE);
		DelayMs(1000);		// pause
		WriteAll((uint8_t*)"abcdefghijklmnopqrstuvwxyz123456"); // se la stringa è più lunga, la scrive dopo
		DelayMs(1000);		// pause
		DisplayLeft(LCD_LINE_VISIBLE);
		DelayMs(1000);		// pause

		PutCommand(RETURN_HOME); DelayMs(5);
		WriteInitial((uint8_t*)"**** LPC");  // WriteString comincia a scrivere a sinistra
		WriteAfter((uint8_t*)"845 ****");       // WriteWord continua a scrivere dalla posizione del cursore
		DelayMs(1000);		// pause

		if (LPC_GPIO_PORT->PIN0 & 0x10) {}
		else DisplayLeft(LCD_LINE_VISIBLE);

		PutCommand(LINE1_HOME);  DelayUs(100);
		Write_ndigitsval( 123456, 8);  // WriteString comincia a scrivere a sinistra
		Write_HDval(69547); // convertito dovrebbe scrivere 10fab
		DelayMs(1000);		// pause

		PutCommand(LINE2_HOME);  DelayUs(100);
		Write_HDval2(69547);

		LED_R //on
		DelayMs(500);
		R_OFF
		DelayMs(500);

		PutCommand(DISPLAY_CLEAR);
		DelayMs(1000);		// pause

  } // end of while(1)
	
} // end of main
