
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

void Delay_sw_ms(uint16_t delayinms)
{
	uint32_t i;
	for(i=0;i<(delayinms*880);i++);
}

/*int main(void) {
	int i;
	unsigned int a;

 	LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<20;   // Turn on clock to GPIO1
    LPC_GPIO_PORT->SET[1] = 1<<1;         // Make the port bit an output driving '1'
    LPC_GPIO_PORT->DIRSET[1]= 1<<1;
    i = 500;
	while(1) {
		LPC_GPIO_PORT->CLR[1] = 1<<1; //LED BLU on

		Delay_sw_ms(i);

		LPC_GPIO_PORT->SET[1] = 1<<1; //LED BLU off
		a = LPC_GPIO_PORT->PIN[0];
		Delay_sw_ms(i);

  } // end of while(1)

} // end of main*/

int main(void) {
	uint16_t timeon;

	LPC_SYSCON->SYSAHBCLKCTRL0 |= (1<<20); // Turn on clock to GPIO1
//  ACT_R
//	ACT_G
//	ACT_B
	ACT_RGB2
	timeon = 1500;
	while(1) {
		LED_R		Delay_sw_ms(timeon);
		LED_G		Delay_sw_ms(timeon);
		LED_B		Delay_sw_ms(timeon);
		RGB_OFF
	} // end of while(1)
} // end of main
