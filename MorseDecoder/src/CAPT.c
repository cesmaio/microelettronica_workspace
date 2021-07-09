#include "LPC8xx.h"

void InitCapT() {
   LPC_SYSCON->SYSAHBCLKCTRL[1] |= 1 << 0;          // Turn on clock to cap
   LPC_SYSCON->SYSAHBCLKCTRL0 |= 1 << 7 | 1 << 18;  //abilito la GPIO1 e SWM e IOCON
   LPC_SYSCON->PRESETCTRL[1] ^= 1 << 0;
   LPC_SYSCON->PRESETCTRL[1] |= 1 << 0;
   LPC_SYSCON->CAPTCLKSEL ^= 0x6;  // clock

   LPC_SWM->PINENABLE0 &= ~(1 << 28);  // enable il CAPT0
   LPC_SWM->PINENABLE1 &= ~(3 << 5);   // should be already enabled by default but no...
   // Enable the fixed-pin functions CAPT_YH, CAPT_YL, and any of CAPT_X0,
   //	CAPT_X1, etc. that will be used, by writing to the appropriate PINENABLE register(s)
   //	in the SWM.
   LPC_IOCON->PIO1_8 ^= (1 << 4);   //disabilito pull-up YL
   LPC_IOCON->PIO1_9 ^= (1 << 4);   //disabilito pull-up YH
   LPC_IOCON->PIO0_31 ^= (1 << 4);  //disabilito pull-up x0
   LPC_IOCON->PIO0_30 ^= (1 << 4);  //disabilito pull-up acmp i5

   LPC_CAPT->CTRL |= 0x02 | (0x7 << 8) | 1 << 16;  // ContPoll 8div act_X0
   LPC_CAPT->POLL_TCNT = 0x200 | 12 << 12 | 1 << 16 | 1 << 24 | 0 << 26 | 1 << 31;

   //	LPC_CAPT->INTENSET = 0x1; // Yestouch
   //	NVIC_EnableIRQ(11);
}
