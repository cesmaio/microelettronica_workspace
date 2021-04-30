/****************************************************************************
 *   $Id:: timer.c 5823 2010-12-07 19:01:00Z usb00423                       $
 *   Project: NXP LPC17xx Timer for PWM example
 *
 *   Description:
 *     This file contains timer code example which include timer 
 *     initialization, timer interrupt handler, and APIs for timer access.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include "LPC8xx.h"
#include "timer.h"

// Variabile globale settata dalla ISR
volatile uint32_t elapsed_time = 0;

void InitMRT0(void)
{
  // *** Configurazione MRT in modalità one-shot per generazione periodi di pausa ***

  // Abilita il clock per MRT
  LPC_SYSCON->SYSAHBCLKCTRL0 |= (1<<10);

  // Reset del modulo
  LPC_SYSCON->PRESETCTRL0 &= ~(1<<10);
  LPC_SYSCON->PRESETCTRL0 |=  (1<<10);

  // MODE: one-shot; interrupt = enable
  LPC_MRT->Channel[0].CTRL = (1<<0) | (1<<1);

  // Connette e abilita l'interrupt dello MRT al NVIC
  NVIC_EnableIRQ(MRT_IRQn);

  // VAL = 0 => MRT0 in idle
  LPC_MRT->Channel[0].INTVAL = 0; //già presente al reset

  // cancella ogni richiesta (spuria?) di interrupt
  LPC_MRT->Channel[0].STAT = 0x1;
}

void delayMs(uint32_t delay_in_ms)
{
	LPC_MRT->Channel[0].INTVAL = (30000000/1000)*delay_in_ms;

	// Aspetta l'handshake dalla ISR
	while(elapsed_time == 0);

	elapsed_time = 0; // resetta flag di handshake
}

void delayUs(uint32_t delay_in_us)
{
	LPC_MRT->Channel[0].INTVAL = (30000000/1000000)*delay_in_us; // questo timer non ha bisogno del -1

	// Aspetta l'handshake dalla ISR
	while(elapsed_time == 0);

	elapsed_time = 0; // resetta flag di handshake
}

// gestione dell'interrupt
void MRT_IRQHandler(void) {

  // Se è '1' il bit 0 delle interrupt flag CH0 ha dato un interrupt
  if (LPC_MRT->IRQ_FLAG & (0x1))
  {
	  elapsed_time = 1;

    // Clear the interrupt flag
    LPC_MRT->Channel[0].STAT = 0x1;
  }

  // Se è '1' il bit 1 delle interrupt flag CH1 ha dato un interrupt
  if (LPC_MRT->IRQ_FLAG & (0x2))
  {
	  if(LPC_GPIO_PORT->PIN[1] & (1<<0)) //se P1.0 alto
		  LPC_GPIO_PORT->CLR[1] = 1<<0;
	  else
		  LPC_GPIO_PORT->SET[1] = 1<<0;

	  // Clear the interrupt flag
      LPC_MRT->Channel[1].STAT = 0x1;
  }

  return;
}
