#include "LPC8xx.h"

void Init_AnComp (char Input1, char Input2)
{
	LPC_SYSCON->SYSAHBCLKCTRL0 |=1<<19 | 1<<7; //Comparator-SWM clock
	LPC_SYSCON->PDRUNCFG ^= 1<<15; // Alimento Comp
	LPC_SYSCON->PRESETCTRL0 ^=1<<19; //Comparator reset
	LPC_SYSCON->PRESETCTRL0 |=1<<19; //Comparator unreset
	LPC_SWM->PINENABLE0 ^= 1<<(Input1-1); //Abilito AInx (Vedere GPIO corrispondente)
	if (Input2) if (Input2<6) LPC_SWM->PINENABLE0 ^= 1<<(Input2-1); // Abilito anche secondo ingresso generico
	LPC_SWM->PINASSIGN11 ^= 0xF<<4; //Abilito PIO0_15 come OUTPUT
	LPC_IOCON->PIO0_15 ^= 0x30; //Disabilito Pull-Up Isteresi
	LPC_CMP->CTRL |= (1<<3) | (1<<6) | (Input1<<8) | (Input2<<11) | (2<<25); // 10 mV di offset
	NVIC_EnableIRQ( CMP_IRQn);
}


/*void CMP_IRQHandler() {
//Write_ndigitsval(i,3);
LPC_CMP->CTRL |= (1<<20);
LPC_CMP->CTRL ^= (1<<20);
}*/



