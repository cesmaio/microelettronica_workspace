#include "LPC8xx.h"

void SysTick_Handler(void)
{
}

void InitDelay(void) {
// SysTick->CTRL |= 0x2; // Attivo Systick Handler
} // end of main

void DelayUs(uint16_t Us) {
	SysTick->LOAD = Us*15-1; // Carico N us come tempo vale N*1e-6*1.5e7 -1 cicli
	// mi rimangono 16-4 12 bit per indicare il tempo in us ma ne bastano 10 per arrivare a 1 ms
	// 3 decadi sono fatte da 10 bit
	SysTick->VAL = 0xFFFFFF;
	SysTick->CTRL |= 0x1; // Attivo Tick Counter Enable
	while(!(SysTick->CTRL & 0x10000));
	SysTick->CTRL ^= 0x1; // Disattivo Tick Counter Enable
} // end of main


void DelayMs(uint32_t Ms) { // Possiamo dare 4 miliardi di Ms cioè 4 Milioni di Sec cioè più di 1000 ore...mica male col timer più semplice
	while (Ms--){
		DelayUs(1000);
	}
} // end of main
