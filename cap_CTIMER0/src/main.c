
/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "HD44780.h"  // gestione LCD
#include "LED.h"
#include "LPC8xx.h"
#include "timer.h"  // funzioni delay

uint32_t delta_t = 0;  //intervallo di tempo tra due pressioni (calcolato nella ISR in base alla cattura)

// Function name: CTimer0_IRQHandler (ISR)
// Description:   calcola intervallo di tempo tra due eventi su CAP0 successive
// Parameters:    Nessuno
// Returns:       Void
void CTIMER0_IRQHandler(void) {
   //  uint32_t valore_attuale;         // valore catturato sull'evento di CAP0
   static uint32_t valore_prec = 0;  // valore precedente catturato dall'evento di CAP0

   if (LPC_CTIMER0->IR & (1 << 4))  //test sulla flag di CAP0
   {
      //	  valore_attuale = LPC_CTIMER0->CR[0];

      delta_t = LPC_CTIMER0->CR[0] - valore_prec;

      valore_prec = LPC_CTIMER0->CR[0];

      for (uint32_t i = 0; i < 300000; i++)
         ;  // breve pausa antirmbalzo

      // Cancella la flag dell'interruzione CAP0
      LPC_CTIMER0->IR = 1 << 4;
   }
}  // end of CTIMER0 ISR

int main(void) {
   uint32_t dummyVal;

   InitMRT0();       //inizializza ch0 di MRT per funzioni delay
   InitLCD_4b_2L();  //initialize LCD

   // Accendi clock per SWM e CTIMER0
   LPC_SYSCON->SYSAHBCLKCTRL[0] |= (1 << 7) | (1 << 25);

   // Configurazione del Counter/Timer Control Register
   // Counter mode: timer mode (clock della periferica)
   LPC_CTIMER0->CTCR = 0x0 << 0;  // come da reset

   LPC_CTIMER0->CCR = (1 << 1)     //cattura TC su fronte di discesa di CAP0
                      | (1 << 2);  //genera interrupt sull'evento di cattura da CAP0

   // Abilita l'interrupt per CTIMER0
   NVIC_EnableIRQ(CTIMER0_IRQn);

   // Prescaler per dividere per 1
   LPC_CTIMER0->PR = 30000000 / 1000 - 1;  // risoluzione 1 ms

   // T0_CAP0 connesso a P0.4 (pulsante USER)
   dummyVal = LPC_SWM->PINASSIGN[14];
   dummyVal &= ~(0xFF << 8);  //azzera bit 8:15
   dummyVal |= (0x04 << 8);   //P0.4 (tasto USER)
   LPC_SWM->PINASSIGN[14] = dummyVal;

   // Abilita CTIME0
   LPC_CTIMER0->TCR = 1 << 0;

   while (1) {
      PutCommand(LINE1_HOME);
      Write_ndigitsval(delta_t / 1000, 6);
      WriteChar('.');
      Write_ndigitsval(delta_t % 1000, 3);
      WriteAfter((uint8_t*)" s");
   }  // end of while(1)
}  // end of main
