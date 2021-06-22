#include "AnComp.h"
#include "LPC8xx.h"
#include "Delay.h"
#include "HD44780.h"
#include "CAPT.h"
#include "10DAC.h"


uint8_t i = 0;

int main ()
{
	InitLCD_4b_2L();
	Init_AnComp( 3 , 7 );
	InitDAC(400,0);
	InitDAC(0,1);

	while(!(LPC_CMP->CTRL & (1<<23))){
		 	 LPC_DAC1->CR = i << 6;
			 LPC_DAC0->CR = (400-i) << 6;
			 i++;
		 }
}

void CMP_IRQHandler() {
Write_ndigitsval(i,3);
LPC_CMP->CTRL |= (1<<20);
LPC_CMP->CTRL ^= (1<<20);
}



