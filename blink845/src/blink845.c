
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

void Delay_sw_ms(uint32_t delayinms)
{
	uint32_t i;
	for(i=0;i<(delayinms*880);i++);
}

int main(void) {
	unsigned int a;

 	LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<20;   // Turn on clock to GPIO1 (pag. 101 user manual)
 	// lasciando il registro invariato (apparte il BIT desiderato) con l'operaiozione di |=
    LPC_GPIO_PORT->SET[1] = 1<<1;         // Make the port bit an output driving '1'
    LPC_GPIO_PORT->DIRSET[1]= 1<<1;

	while(1) {
		LPC_GPIO_PORT->CLR[1] = 1<<1; //LED BLU on (Clear signal HIGH)

		Delay_sw_ms(200);

		LPC_GPIO_PORT->SET[1] = 1<<1; //LED BLU off

		a = LPC_GPIO_PORT->PIN[0];

		Delay_sw_ms(200);

  } // end of while(1)

} // end of main
