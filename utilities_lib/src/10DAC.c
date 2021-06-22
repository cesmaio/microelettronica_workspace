#include "LPC8xx.h"


void InitDAC (uint16_t value, char channel) {
	if (channel)
		{LPC_SYSCON->PDRUNCFG ^= 1<<14; // Alimento DAC1
		LPC_SYSCON->SYSAHBCLKCTRL1 |= 0x02;//abilito il clock
		LPC_SYSCON->SYSAHBCLKCTRL0 |=1<<18 ;//abilito il clock IOCON
		LPC_SWM->PINENABLE0 ^= 1<<27; //Abilito PIO0_29
		LPC_IOCON->PIO0_29 = 1<<16; //Abilito analogica
		LPC_DAC1->CR = (value & 0x3FF) << 6;
		}
	else
		{LPC_SYSCON->PDRUNCFG ^= 1<<13; // Alimento DAC0
		LPC_SYSCON->SYSAHBCLKCTRL0 |= 1<<27 ; //abilito clock
		LPC_SYSCON->SYSAHBCLKCTRL0 |=1<<18;//abilito il clock IOCON
		LPC_SWM->PINENABLE0 ^= 1<<26; //Abilito PIO0_17
		LPC_IOCON->PIO0_17 = 1<<16; //Abilito analogica
		LPC_DAC0->CR = (value & 0x3FF) << 6;
		}
}



