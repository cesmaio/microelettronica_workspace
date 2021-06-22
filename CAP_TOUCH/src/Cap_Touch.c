#include "LPC8xx.h"
#include "LED.h"
#include "CAPT.h"
#include  "Delay.h"



int main (void)
{
	LPC_SYSCON->SYSAHBCLKCTRL0 |= 1<<20; //abilito il GPIO1
	InitCapT();
	ACT_R

	while(1){
		if(LPC_CAPT->TOUCH  & 1<<16)
			{LED_R
			DelayMs(1000);}
		else
			{R_OFF;
			DelayMs(1000);}
	}
}




