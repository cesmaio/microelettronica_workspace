
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
#include "Delay.h"
#include "HD44780.h"

int main(void) {
uint32_t a;

	LPC_SYSCON->SYSAHBCLKCTRL0 |= ((1<<20) | (1<<6)); // Turn on clock to GPIO1 & GPIO0
	// lasciando il registro invariato apparte il BIT desiderato con l'operazione di OR uguale (|=)
	ACT_G
	LPC_GPIO_PORT->DIRSET0 = (1<<15);
		LPC_SYSCON->SYSAHBCLKCTRL[0] |=  ((1<<25) | (1<<7)); // Accendi clock per CTIMER0 & SWM
	LPC_SWM->PINASSIGN14 = (0xF0404FFF); // Connetto al Led Verde CAP0 e CAP1, SWM è HIGH
	LPC_CTIMER0->CCR = 0xA; //Falling edge in CAP0 e RE in CAP1
	LPC_CTIMER0->TCR = 0x2;
	LPC_CTIMER0->CTCR |= 0x0;   // come da reset
	LPC_CTIMER0->TCR = 0x1;
	while(1) {
		LED_G // Verde Acceso
		LPC_GPIO_PORT->NOT0 = (1<<15);;
		DelayMs(1000);
		G_OFF // Verde spento
		LPC_GPIO_PORT->NOT0 = (1<<15);
		DelayMs(1000);
		a = LPC_CTIMER0->CR[1] - LPC_CTIMER0->CR[0];
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
