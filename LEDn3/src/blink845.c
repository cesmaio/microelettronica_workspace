
/*
===============================================================================
 Name        : Blinky.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include "LPC8xx.h"
#include "LED.h"
uint32_t button_count, press, rimbalzo;

int main(void) {

	LPC_SYSCON->SYSAHBCLKCTRL0 |= (1<<20); // Turn on clock to GPIO1
	// lasciando il registro invariato apparte il BIT desiderato con l'operazione di OR uguale (|=)
//  ACT_R
//	ACT_G
//	ACT_B // Premendo CTRL + SPACE voi attivate tutte le strighe di comando riconosciute dal sistema
	ACT_R
	ACT_G
	while(1) {
		if (LPC_GPIO_PORT->PIN1 & 0x2)      //111111111111
		{R_OFF LED_G	// pulsante premuto    //000000000010
			if (press == 1) rimbalzo++;} //000000000010 ~(000000000010)=11111111101 if(11111111101)--> entra nel ciclo
		else
		{G_OFF LED_R	if (++button_count==1) press++;}
	} // end of while(1)
} // end of main

/* LPC_GPIO_PORT->CLR0 |= 0x2; // se scriviamo un valore alto nel reg di clear , noi abbassiamo il valore del pin
LPC_GPIO_PORT->DIRSET0 |= 0x2; // la attiviamo come uscita
ACT_RGB
while(1) {
	if(LPC_GPIO_PORT->PIN0 & 1)
	{R_OFF  LPC_GPIO_PORT->CLR0 |= 2;   LED_G}
	else
	{LED_R  LPC_GPIO_PORT->SET0 |= 2;} */
