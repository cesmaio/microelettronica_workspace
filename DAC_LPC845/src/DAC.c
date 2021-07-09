#include "10DAC.h"
#include "AnComp.h"
#include "CAPT.h"
#include "Delay.h"
#include "HD44780.h"
#include "LPC8xx.h"

uint8_t i = 0;

int main() {
   InitLCD_4b_2L();
   Init_AnComp(3, 7);
   InitDAC(400, 0);
   InitDAC(0, 1);

   while (!(LPC_CMP->CTRL & (1 << 23))) {  // COMPEDGE: quando viene rilevato l'edge di salita o discesa, esce dal while
                                           // incremento il DAC1 e decremento il DAC0:
                                           // l'uscita del Comparatore diventa alta quando il sengale di DAC1 super DAC0
      LPC_DAC1->CR = i << 6;
      LPC_DAC0->CR = (400 - i) << 6;  // ~1.3V
      i++;                            // analog value
   }
}

void CMP_IRQHandler() {
   Write_ndigitsval(i, 3);
   LPC_CMP->CTRL |= (1 << 20);
   LPC_CMP->CTRL ^= (1 << 20);
}
