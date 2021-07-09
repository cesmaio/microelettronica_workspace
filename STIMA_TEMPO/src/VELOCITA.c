// #include "Delay.h"
#include "HD44780.h"
#include "LED.h"
#include "LPC8xx.h"
#include "timer.h"  // funzioni delay

unsigned int dummyVal = 0, v_km_h = 0;
uint32_t tempo1 = 0, delta_t = 0;
unsigned char misura_iniziata, misura_finita, first_event, rebounds, measurements;

int main(void) {
   unsigned int dummyVal;

   InitLCD_4b_2L();  //initialize LCD

   LPC_SYSCON->SYSAHBCLKCTRL[0] |= (1 << 7) | (1 << 25);  // Accendi clock per SWM e CTIMER0

   LPC_CTIMER0->CCR = 0x3F;  //Rising Edge and FAlling Edge e INT. di CAP[0] e CAP[1]

   LPC_CTIMER0->TCR = 1 << 1;  // Reset dei registri

   NVIC_EnableIRQ(CTIMER0_IRQn);  // Abilita l'interrupt di CTIMER0 sulla CPU

   LPC_CTIMER0->PR = 30000000 / 1000 - 1;  // 1ms di periodo

   // TO_CAP0 connesso a P0.4 (pulsante USER)
   dummyVal = LPC_SWM->PINASSIGN[14];
   dummyVal &= ~(0xFF << 8);  //azzera bit 8:15
   dummyVal |= (0x04 << 8);   //P0.4 (tasto USER)
   LPC_SWM->PINASSIGN[14] = dummyVal;

   // TO_CAP1 connesso a P0.8 (pulsante esterno)
   dummyVal = LPC_SWM->PINASSIGN[14];
   dummyVal &= ~(0xFF << 16);  //azzera bit 8:15
   dummyVal |= (0x08 << 16);   //P0.8
   LPC_SWM->PINASSIGN[14] = dummyVal;

   LPC_CTIMER0->TCR = 1 << 0;  // Abilita CTIMER0

   misura_iniziata = 0;
   misura_finita = 0;
   first_event = 1;
   rebounds = 0;
   measurements = 0;

   PutCommand(LINE1_HOME);
   WriteInitial((uint8_t*)"     Ready      ");

   while (1) {
      PutCommand(LINE2_HOME);
      if (misura_iniziata)  //ha iniziato la misura
      {
         PutCommand(LINE1_HOME);
         WriteInitial((uint8_t*)"  Measurement   ");
      }
      if (misura_finita) {
         v_km_h = ((3600000 / delta_t) + 5) / 10;  //  V = 3.6e6 m/ms (supponiamo una distanza di 10 m tra i due segnali)
         // attenzione si moltplica per 100 per avere due cifre decimali
         PutCommand(LINE2_HOME);
         WriteAfter((uint8_t*)"Speed:");
         Write_ndigitsval_space(v_km_h / 10, 3);
         WriteAfter(".");
         Write_ndigitsval(v_km_h % 10, 1);
         WriteAfter((uint8_t*)" km/h");

         misura_iniziata = 0;
         measurements++;
         WriteInitial((uint8_t*)"     Ready      ");
         PutCommand(LINE1_HOME);
         DelayMs(100);
         first_event = 1;
      }

      // reset dei registri per evitare di superare 24bit
      if (LPC_CTIMER0->TC > 0xFFFFFFF) {
         LPC_CTIMER0->TCR = 1 << 1;
         LPC_CTIMER0->TCR = 1 << 0;  // riabilita il conteggio
      }

   }  // end of while(1)
}  // end of main

void CTIMER0_IRQHandler(void) {
   if (first_event && LPC_CTIMER0->IR & (1 << 4)) {
      tempo1 = LPC_CTIMER0->CR[0];
      misura_iniziata = 1;
      misura_finita = 0;
      first_event = 0;
      LPC_CTIMER0->IR = 1 << 4;  // Bisogna eliminare l'interrupt
   }
   if (misura_iniziata && LPC_CTIMER0->IR & (1 << 5)) {
      delta_t = LPC_CTIMER0->CR[0] - tempo1;
      misura_finita = 1;
      misura_iniziata = 0;
      DelayMs(100);              // breve pausa antirmbalzo
      LPC_CTIMER0->IR = 1 << 5;  // Bisogna eliminare l'interrupt
   }
}