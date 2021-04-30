#include "LPC8xx.h"
#include "HD44780.h"
#include "Delay.h"
#include "LED.h"

unsigned int dummyVal, delta_t, valore_prec, v_km_h;
unsigned char misura_iniziata;

int main(void) {
	unsigned int dummyVal;

	InitLCD_4b_2L(); //initialize LCD

	LPC_SYSCON->SYSAHBCLKCTRL[0] |= (1<<7) | (1<<25);  // Accendi clock per SWM e CTIMER0

	LPC_CTIMER0->CCR = 0x3; //fronte negativo di CAP[0] e INT.PT

	LPC_CTIMER0->TCR = 1<<1; // Reset dei registri

	NVIC_EnableIRQ(CTIMER0_IRQn); // Abilita l'interrupt di CTIMER0 sulla CPU

	LPC_CTIMER0->PR = 30000000/1000 - 1; // 1ms di periodo

	// T0_CAP0 connesso a P0.4 (pulsante USER)
	dummyVal = LPC_SWM->PINASSIGN[14];
	dummyVal &= ~(0xFF<<8); //azzera bit 8:15
	dummyVal |= (0x04 << 8); //P0.4 (tasto USER)
	LPC_SWM->PINASSIGN[14] = dummyVal;

	LPC_CTIMER0->TCR = 1<<0;// Abilita CTIMER0

	 misura_iniziata = 0;

	 while(1){
		 PutCommand(LINE2_HOME);
		 if(misura_iniziata) //ha iniziato la misura
		 {
			 PutCommand(LINE1_HOME); WriteInitial((uint8_t*)" Ready ");
			 PutCommand(LINE2_HOME); WriteAfter((uint8_t*)"       ");
		 }
		 else
		 {
			 PutCommand(LINE1_HOME); WriteInitial((uint8_t*)"  Measurement   ");
			 PutCommand(LINE2_HOME); WriteAfter((uint8_t*)"Speed:");
			 v_km_h = ( (360000000/delta_t) + 5 )/10; //  V = 3.6e6 m/ms
			 // attenzione si moltplica per 100 per avere due cifre decimali
			 Write_ndigitsval(v_km_h/10, 3); WriteAfter('.');
			 Write_ndigitsval( v_km_h%10, 1);
			 WriteAfter((uint8_t*)" km/h");
		}

	 } // end of while(1)
} // end of main

void CTIMER0_IRQHandler(void)
{
	if(misura_iniziata &&  (LPC_CTIMER0->IR & (1<<4)) ){
		delta_t = LPC_CTIMER0->CR[0] - valore_prec;
		misura_iniziata = 0;
	//	DelayUs(100);
	}
	else { if (LPC_CTIMER0->IR & (1<<4)) {
		valore_prec = LPC_CTIMER0->CR[0]; misura_iniziata = 1;}
	}
	LPC_CTIMER0->IR = 1<<4; // Bisogna eliminare l'interrupt
}

