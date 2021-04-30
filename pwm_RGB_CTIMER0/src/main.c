
/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "LPC8xx.h"
#include "Delay.h"
#include "HD44780.h"

// Inizializza duty cycle RED, GREEN e BLU
uint8_t dc_red = 0, dc_green = 0, dc_blu = 0;

/*int main(void) {

	uint32_t dummyVal;

	uint8_t switch_USER_pressed = 0, switch_SCELTA_pressed = 0;
	uint8_t scelta = 0;

	// GPIO 0 : pulsanti USER e ISP
	LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<6;   // Turn on clock to GPIO0

	InitLCD_4b_2L();

	// Accendi clock per SWM e CTIMER0
	LPC_SYSCON->SYSAHBCLKCTRL[0] |= (1<<7) | (1<<25);

	// Configurazione del Count Control Register
	// Counter mode: timer mode (clock della periferica)
	LPC_CTIMER0->CTCR = 0x0<<0; // come da reset

	// *** Connessione delle uscite Match/PWM di CTIMER0 ***

	// T0_MAT0 connesso a led RED (P1.2)
	dummyVal = LPC_SWM->PINASSIGN[13];
	dummyVal &= ~(0xFF<<8); //azzera bit 8:15
	dummyVal |= (0x22 << 8); //P1.2 (led RED)
	LPC_SWM->PINASSIGN[13] = dummyVal;

	// T0_MAT1 connesso a led verde (P1.0)
	dummyVal = LPC_SWM->PINASSIGN[13];
	dummyVal &= ~(0xFF<<16); //azzera bit 23:16
	dummyVal |= (0x20 << 16); //P1.0 (led GREEN)
	LPC_SWM->PINASSIGN[13] = dummyVal;

	// T0_MAT2 connesso a led blu (P1.1)
	dummyVal = LPC_SWM->PINASSIGN[13];
	dummyVal &= ~(0xFF<<24); //azzera bit 31:24
	dummyVal |= (0x21 << 24); //P1.1 (led BLU)
	LPC_SWM->PINASSIGN[13] = dummyVal;

	// Imposta il periodo del PWM in percentuale D.C.: 100 cicli di Timer Counter
	LPC_CTIMER0->MR[3] = 100; //periodo voluto 3 ms, quindi singolo clock 30 us
	// Prescaler per dividere per risoluzione di 0.03 ms
	LPC_CTIMER0->PR = 30000000/100000/3 - 1; //un incremento di TC ogni 30 us

    // Match Control register: interrupt e reset sul match MR3: ad ogni periodo viene cambiato il duty cycle
	LPC_CTIMER0->MCR = (1<<9)|(1<<10);

	// Uscite Match 0, 1 e 2: PWM mode
	LPC_CTIMER0->PWMC = (1<<0) | (1<<1) | (1<<2);

	// Abilita l'interrupt per CTIMER0
	NVIC_EnableIRQ(CTIMER0_IRQn);

	// Abilita CTIMER0
	LPC_CTIMER0->TCR = 1<<0;

	while(1) {
		WriteInitial((uint8_t*)"R: ");
		Write_ndigitsval(dc_red, 3);
		WriteAfter((uint8_t*)" G: ");
		Write_ndigitsval(dc_green, 3);
		PutCommand(LINE2_HOME);
		WriteAfter((uint8_t*)"B: ");
		Write_ndigitsval(dc_blu, 3);
		if(scelta == 0)
		   WriteAfter((uint8_t*)"  * RED * ");
		if(scelta == 1)
		   WriteAfter((uint8_t*)"  *GREEN* ");
		if(scelta == 2)
		   WriteAfter((uint8_t*)"  * BLU * ");

		if( ( (LPC_GPIO_PORT->PIN[0] & (1<<16) ) == 0 ) && (switch_SCELTA_pressed == 0) )//tasto SCELTA premuto
		{
			switch_SCELTA_pressed = 1;
			scelta++;
			if(scelta > 2)  scelta = 0;
		}
		if( ( (LPC_GPIO_PORT->PIN[0] & (1<<16) ) ) && (switch_SCELTA_pressed) )//tasto ISP rilasciato
		{
			switch_SCELTA_pressed = 0;
		}

		if( ( (LPC_GPIO_PORT->PIN[0] & (1<<4) ) == 0 ) && (switch_USER_pressed == 0) )//tasto USER premuto MESSO ANCHE ESTERNAMENTE
		{
			switch_USER_pressed = 1;

			if(scelta == 0)
			{  dc_red += 10;
			   if(dc_red > 100)  dc_red = 0;
			}

			if(scelta == 1)
			{  dc_green+=10;
			   if(dc_green > 100)  dc_green = 0;
			}

			if(scelta == 2)
			{  dc_blu+=10;
			   if(dc_blu > 100)  dc_blu = 0;
			}
		}
		if( ( (LPC_GPIO_PORT->PIN[0] & (1<<4) ) ) && (switch_USER_pressed) )//tasto USER rilasciato
		{
			switch_USER_pressed = 0;
		}

	} // end of while(1)

}// end of main */


// Function name: CTimer0_IRQHandler (ISR)
// Description:   Aggiorna il duty cycle di PWM0
// Parameters:    Nessuno
// Returns:       Void
void CTIMER0_IRQHandler(void)
{
//  static uint32_t duty_cycle = 0; // duty cycle: 0-100, step 1
	  LPC_CTIMER0->MR[0] = dc_red;
	  LPC_CTIMER0->MR[1] = dc_red + dc_green;
	  LPC_CTIMER0->MR[2] = dc_red + dc_green + dc_blu;
  // Cancella la flag dell'interruzione (Match 3)
  LPC_CTIMER0->IR = 1<<3;
} // end of CTIMER0 ISR

int main(void) {

	uint32_t dummyVal=0, red_addr=0, scelta_addr=0, green_addr=0, blu_addr=0;
	uint32_t switch_DC = 0, switch_COLORE = 0; // Attenzione partire con switch alti per evitare inizializzazione non voluta
	uint8_t scelta = 0, dir = 1;

	InitLCD_4b_2L();

	// Accendi clock per SWM e CTIMER0
	LPC_SYSCON->SYSAHBCLKCTRL[0] |= (1<<7) | (1<<25);

	// Configurazione del Count Control Register
	// Counter mode: timer mode (clock della periferica)
	LPC_CTIMER0->CTCR = 0x0<<0; // come da reset

	// *** Connessione delle uscite Match/PWM di CTIMER0 ***

	// T0_MAT0 connesso a led RED (P1.2)
	dummyVal = LPC_SWM->PINASSIGN[13];
	dummyVal &= ~(0xFF<<8); //azzera bit 8:15
	dummyVal |= (0x22 << 8); //P1.2 (led RED)
	LPC_SWM->PINASSIGN[13] = dummyVal;

	// T0_MAT1 connesso a led verde (P1.0)
	dummyVal = LPC_SWM->PINASSIGN[13];
	dummyVal &= ~(0xFF<<16); //azzera bit 23:16
	dummyVal |= (0x20 << 16); //P1.0 (led GREEN)
	LPC_SWM->PINASSIGN[13] = dummyVal;

	// T0_MAT2 connesso a led blu (P1.1)
	dummyVal = LPC_SWM->PINASSIGN[13];
	dummyVal &= ~(0xFF<<24); //azzera bit 31:24
	dummyVal |= (0x21 << 24); //P1.1 (led BLU)
	LPC_SWM->PINASSIGN[13] = dummyVal;

	// Imposta il periodo del PWM in percentuale D.C.: 100 cicli di Timer Counter
	LPC_CTIMER0->MR[3] = 100; //periodo voluto 10 ms, quindi singolo clock 30 us
	// Prescaler per dividere per risoluzione di 0.1 ms
	LPC_CTIMER0->PR = 30000000/100000 - 1; //un incremento di TC ogni 100 us

    // Match Control register: interrupt e reset sul match MR3: ad ogni periodo viene cambiato il duty cycle
	LPC_CTIMER0->MCR = (1<<9)|(1<<10);

	// Uscite Match 0, 1 e 2: PWM mode
	LPC_CTIMER0->PWMC = (1<<0) | (1<<1) | (1<<2);

	// Abilita l'interrupt per CTIMER0
	NVIC_EnableIRQ(CTIMER0_IRQn);

	// Abilita CTIMER0
	LPC_CTIMER0->TCR = 1<<0;

	WriteInitial((uint8_t*)"R: ");
	Write_ndigitsval_space(dc_red, 3);
	WriteAfter((uint8_t*)" G: ");
	Write_ndigitsval_space(dc_green, 3);
	PutCommand(LINE2_HOME);
	WriteAfter((uint8_t*)"B: ");
	Write_ndigitsval_space(dc_blu, 3);
	if(scelta == 0)  WriteAfter((uint8_t*)"  * RED * ");
	if(scelta == 1)  WriteAfter((uint8_t*)"  *GREEN* ");
	if(scelta == 2)  WriteAfter((uint8_t*)"  * BLU * ");
	red_addr = 0x80 | 0x03;
	blu_addr = 0x80 | 0x43;
	scelta_addr = 0x80 | 0x48;
	green_addr = 0x80 | 0x0A;
	PutCommand(DISP_ON_CUR_OFF_BLINK_OFF);

	while(1) {
		dir = 1;

		if(scelta == 0)
			{PutCommand(red_addr);
			Write_ndigitsval_space(dc_red, 3);
			PutCommand(scelta_addr);
			WriteAfter((uint8_t*)"* RED *");}
		if(scelta == 1)
			{PutCommand(green_addr);
			Write_ndigitsval_space(dc_green, 3);
			PutCommand(scelta_addr);
			WriteAfter((uint8_t*)"*GREEN*");}
		if(scelta == 2)
			{PutCommand(blu_addr);
			Write_ndigitsval_space(dc_blu, 3);
			PutCommand(scelta_addr);
			WriteAfter((uint8_t*)"* BLU *");}

		// Se potenziometro alto Si cambia direzione di incremento

		if (!(LPC_GPIO_PORT->PIN[0] & (1<<4))) {
			dir=0;
		}

		if( (LPC_GPIO_PORT->PIN[0] & (1<<16)) == (switch_COLORE & (1<<16)) )//tasto COLORE premuto Oppure Rilasciato
		{
			switch_COLORE = ~(LPC_GPIO_PORT->PIN[0] & (1<<16));
			scelta++;
			if(scelta > 2)  scelta = 0;
		}

		if ((LPC_GPIO_PORT->PIN[0] & (1<<18)) == (switch_DC & (1<<18)) )//tasto per il D.C. esterno
		{
			switch_DC = ~(LPC_GPIO_PORT->PIN[0] & (1<<18));

			if(scelta == 0)
			{  if(dir) dc_red += 10;
				else dc_red -= 10;
			   if(dc_red + dc_green + dc_blu > 100)  dc_red -= 10;
			   if(dc_red > 101)  dc_red = 0;
			}

			if(scelta == 1)
			{  if(dir) dc_green += 10;
				else dc_green -= 10;
			   if(dc_red + dc_green + dc_blu > 100)  dc_green -= 10;
			   if(dc_green > 101)  dc_green = 0;
			}

			if(scelta == 2)
			{  if(dir) dc_blu += 10;
				else dc_blu -= 10;
			   if(dc_red + dc_green + dc_blu > 100)  dc_blu -= 10;
			   if(dc_blu > 101)  dc_blu = 0;
			}
		}

	} // end of while(1)

} // end of main

/* void CTIMER0_IRQHandler(void)
{
// fissa duty cycle per RED
	  LPC_CTIMER0->MR[0] = dc_red;
	  LPC_CTIMER0->MR[1] = dc_green;
	  LPC_CTIMER0->MR[2] = dc_blu;

	  // Cancella la flag dell'interruzione (Match 3)
  LPC_CTIMER0->IR = 1<<3;

} // end of CTIMER0 ISR */
