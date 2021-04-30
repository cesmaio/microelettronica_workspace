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
// Function name: CTimer0_IRQHandler (ISR)
// Description:   Aggiorna il duty cycle di PWM0
// Parameters:    Nessuno
// Returns:       Void

 

void CTIMER0_IRQHandler(void)
{
  static int8_t duty_cycle = 0; // duty cycle: 0-100, step 1
  static uint8_t dir = 1;       //1: aumenta intensità, 0: diminuisce intensità

  // Aggiorna il duty-cycle di PWM-MR0
  if (dir)
  {  // Se l'intensità sta aumentando:
     LPC_CTIMER0->MR[0] = duty_cycle++; //post decremento (al max viene caricato 100 in MR[0]
     if (duty_cycle == 101) {
         duty_cycle = 98;
         dir = 0;
     }
  }
  else
  {  // Altrimenti, l'intensità sta diminuendo:
     LPC_CTIMER0->MR[0] = duty_cycle;
      duty_cycle -= 2;
     if (duty_cycle == 0)
        dir = 1;
  }
  // Cancella la flag dell'interruzione (Match 3)
  LPC_CTIMER0->IR = 1<<3;
} // end of Ctimer0 ISR

int main(void) {
    uint32_t dummyVal;
    // Accendi clock per SWM e CTIMER0
    LPC_SYSCON->SYSAHBCLKCTRL[0] |= (1<<7) | (1<<25);
    // Configurazione del Count Control Register
    // Counter mode: timer mode (clock della periferica)
    LPC_CTIMER0->CTCR = 0x0<<0; // come da reset

    // *** Connessione delle uscite Match/PWM di CTIMER0 ***
    // T0_MAT0 connesso a led verde (P1.0)
    dummyVal = LPC_SWM->PINASSIGN[13];
    dummyVal &= ~(0xFF<<8); //azzera bit 8:15
    dummyVal |= (0x20 << 8); //P1.0 (led verde) (es. 0x22 -> led rosso)
    LPC_SWM->PINASSIGN[13] = dummyVal;
    // Prescaler per dividere per risoluzione di 0.1 ms
    LPC_CTIMER0->PR = 30000000/10000 - 1; //un incremento di TC ogni 100 us
    // Imposta il periodo del PWM: 100 cicli di Timer Counter
    LPC_CTIMER0->MR[3] = 100; //periodo: 100x100 us = 10 ms
    // Match Control register: interrupt e reset sul match MR3: ad ogni periodo viene cambiato il duty cycle
    LPC_CTIMER0->MCR = (1<<9)|(1<<10);
    // Uscita Match 0: PWM mode
    LPC_CTIMER0->PWMC = (1<<0);
    // Abilita l'interrupt per CTIMER0
    NVIC_EnableIRQ(CTIMER0_IRQn);
    // Abilita CTIME0
    LPC_CTIMER0->TCR = 1<<0;

    while(1) {
        // non fa nulla!
  } // end of while(1)
} // end of main