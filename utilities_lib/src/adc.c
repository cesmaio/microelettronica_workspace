/*
 * adc.c
 *
 *  Created on: 28 mag 2021
 *      Author: Orso
 */
#include "adc.h"
#include "LPC8xx.h"

void InitADC(uint8_t channel)
{
	uint32_t clk_div;

	  // 1. Power-up e reset dell'ADC; abilita e seleziona il clock
		// Power-up dell'ADC
		LPC_SYSCON->PDRUNCFG &= ~(1<<4);

		// Setta il reset per l'ADC
		LPC_SYSCON->PRESETCTRL0 &= ~(1<<24);
		// Togli reset all'ADC
		LPC_SYSCON->PRESETCTRL0 |= (1<<24);

		// Abilita clock: ADC, SWM, IOCON (periferiche utilizzate)
		LPC_SYSCON->SYSAHBCLKCTRL[0] |= (1<<24) | (1<<7) | (1<<18);

		// Divisore di clock per l'ADC pari a 1 (abilita clock perché diverso da zero)
		LPC_SYSCON->ADCCLKDIV = 1;

		// Usa FRO-clk come sorgente di clock dell'ADC
		LPC_SYSCON->ADCCLKSEL = 0;


	  // 2. Self-calibration
		SystemCoreClockUpdate(); //funzione che resistituisce in variabili globali i valori di frequenza di clock
		// Impostare CLKDIV a 500 kHz
		clk_div = (fro_clk / 500000) - 1;

		// Inizia auto-calibrazione
		// CALMODE = 1; Low Power Mode = 0; CLKDIV = per avere 500 kHz
		LPC_ADC->CTRL = ( (1<<30) | (0<<10) | (clk_div<<0) );

		// Il bit 30 viene autamicamente azzerato non appena finisce la auto-calibrazione
		while (LPC_ADC->CTRL & (1<<30)); //Aspetta fine calibrazione


	  // 3. Configura i pin utilizzati

		LPC_SWM->PINENABLE0 &= ~(1<<(14+channel));// Si abilita ADC : Il trimmer è collegato a ADC0 = PIO0.7

		// Abilita il clock per IOCON
		LPC_SYSCON->SYSAHBCLKCTRL0 |= (1<<18);
		switch(channel )
		{ case 0: LPC_IOCON->PIO0_7 &= ~(3<<3); // Disattiva il pull-up e pull-down per PIO0.7
		case 1: LPC_IOCON->PIO0_6 &= ~(3<<3);
		case 2: LPC_IOCON->PIO0_14 &= ~(3<<3);
		case 3: LPC_IOCON->PIO0_23 &= ~(3<<3);
		case 4: LPC_IOCON->PIO0_22 &= ~(3<<3);
		case 5: LPC_IOCON->PIO0_21 &= ~(3<<3);
		case 6: LPC_IOCON->PIO0_20 &= ~(3<<3);
		case 7: LPC_IOCON->PIO0_19 &= ~(3<<3);
		case 8: LPC_IOCON->PIO0_18 &= ~(3<<3);
		case 9: LPC_IOCON->PIO0_17 &= ~(3<<3);
		case 10: LPC_IOCON->PIO0_13 &= ~(3<<3);
		case 11: LPC_IOCON->PIO0_4 &= ~(3<<3);
		}

	  // 4. Configura il "tipo" di tensione di alimentazione dell'ADC (3.3 V per la LPC845-BRK)
		LPC_ADC->TRM &= ~(1<<5); // '0' for high voltage


	  // 5. Scelta del valore di clock per l'ADC
	  //    La conversione completa richiede 25 periodi di clock.
	  //    Con 30 MHz (CLKDIV = 0) -> 1.2 MSPS.
	  //    SystemCoreClock può essere usata per calcolare la frequenza desiderata.
		#define ADC_CLK_FREQ 1200000 //Definisco una frequenza di 12 MHz
		clk_div = (fro_clk / (25 * ADC_CLK_FREQ)) - 1;

		// CALMODE = 0; Low Power Mode = 0; CLKDIV = per avere il valore deisderato di frequenza (ADC_CLK_FREQ)
		LPC_ADC->CTRL = ( (0<<30) | (0<<10) | (clk_div<<0) );


	  // 6. Selezione della sorgente di trigger: via SOFTWARE
		// Scegli ADC0 in CHANNELS
		LPC_ADC->SEQA_CTRL |= 1<<channel;
		// TRIGPOL = 1 e SEQ_ENA = 1
		LPC_ADC->SEQA_CTRL |= (1<<18) | (1<<31);


	  // 7. disabilita IRQ da ADC
		LPC_ADC->INTEN = 0;

		return;
}

uint16_t ReadADC(uint8_t channel, uint16_t readings)
{
	uint32_t value = 0;

	  for(uint8_t i=0;i<readings;i++)
	  {
		  // AVVIA CONVERSIONE
		  LPC_ADC->SEQA_CTRL |= 1<<26;

/*		  if(channel == 0)
		  {
			  while( (LPC_ADC->DAT[channel] & (1<<31) ) == 0); // Attesa di fine conversione
			  value += (LPC_ADC->DAT[0] >> 4) & 0xFFF; // legge il valore dell'ADC
		  }
		  if(channel == 1)
		  {
			  while( (LPC_ADC->DAT[1] & (1<<31) ) == 0); // Attesa di fine conversione
			  value += (LPC_ADC->DAT[1] >> 4) & 0xFFF; // legge il valore dell'ADC
		  }*/
		  while( (LPC_ADC->DAT[channel] & (1<<31) ) == 0);
		  value += (LPC_ADC->DAT[channel] >> 4) & 0xFFF;
	  }

	  return value/readings;
}
