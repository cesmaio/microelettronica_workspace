
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
#include "timer.h"

int main(void) {

	LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<20;   // Turn on clock to GPIO1
//    LPC_GPIO_PORT->SET[1] = 1<<1;         // Make the port bit an output driving '1'
//    LPC_GPIO_PORT->DIRSET[1]= 1<<1;

    // Enable clocks to CTIMER0 (funzioni delay)
    LPC_SYSCON->SYSAHBCLKCTRL0 |= (1<<25);

    InitMRT0(); //inizializza ch0 di MRT per funzioni delay
	InitLCD(); //initialize LCD

	while(1) {
		PutCommand(DISPLAY_CLEAR); delayMs(5);
		PutCommand(RETURN_HOME);   delayMs(5);
		delayMs(1000);		// pause

		WriteString((uint8_t*)"LP", 0); // scrive una stringa sulla riga 0 (16 caratteri)
		WriteChar('C');                  // scrive un carattere
		WriteChar(NUM_TO_CODE(8));       // scrive il carattere ASCII relativo a un numero
		Write_2digitsval(45);            // scrive un valore decimale a due cifre
		delayMs(1000);		// pause

		WriteString((uint8_t*)"***test LCD 1602", 1); // scrive una stringa sulla riga 1

		PutCommand(RETURN_HOME);   delayMs(5); //non cancella le scritte presenti ma torna solo con cursore in alto a sinistra
		WriteString((uint8_t*)"abcdefghijklmnopqrstuvwxyz123456", 0); // se la stringa è più lunga, va a capo automaticamente
		delayMs(1000);		// pause

		PutCommand(RETURN_HOME);   delayMs(5); //non cancella le scritte presenti ma torna solo con cursore in alto a sinistra
		WriteString((uint8_t*)"**** LPC", 0);  // WriteString comincia a scrivere a sinistra
		WriteWord((uint8_t*)"845 ****");       // WriteWord continua a scrivere dalla posizione del cursore
		delayMs(1000);		// pause

		PutCommand(RETURN_HOME);   delayMs(5); //non cancella le scritte presenti ma torna solo con cursore in alto a sinistra
		Write_ndigitsval( 12345678, 8);  // WriteString comincia a scrivere a sinistra
		WriteWord((uint8_t*)"        ");       // WriteWord continua a scrivere dalla posizione del cursore
		delayMs(1000);		// pause

		LPC_GPIO_PORT->CLR[1] = 1<<1; //LED RED on
		delayMs(500);

		LPC_GPIO_PORT->SET[1] = 1<<1; //LED RED off
		delayMs(500);

  } // end of while(1)
	
} // end of main
