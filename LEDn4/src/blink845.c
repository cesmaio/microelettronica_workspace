
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

int main(void) {
uint32_t a;

	LPC_SYSCON->SYSAHBCLKCTRL0 |= (1<<20); // Turn on clock to GPIO1 & GPIO0
	// lasciando il registro invariato apparte il BIT desiderato con l'operazione di OR uguale (|=)
	ACT_G
	LPC_SYSCON->SYSAHBCLKCTRL0 |= (1<<10); //attivazione del timer
	LPC_MRT->Channel[0].INTVAL = LPC_MRT->Channel[0].TIMER; //INTVALn register is updated in the idle state.
	LPC_MRT->Channel[0].CTRL = 0x1;
//	a = LPC_MRT->Channel[0].INTVAL - LPC_MRT->Channel[0].TIMER;
	while(1) {
		a = LPC_MRT->Channel[0].TIMER;
//		G_OFF // Verde spento
		DelayMs(1000);
//		LED_G // Verde Acceso
		DelayMs(1000);
		a -= LPC_MRT->Channel[0].TIMER;
		DelayMs(1);
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
