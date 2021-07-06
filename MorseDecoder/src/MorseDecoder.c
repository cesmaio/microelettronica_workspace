#include "CAPT.h"
#include "LED.h"
#include "LPC8xx.h"

int main(void) {
   LPC_SYSCON->SYSAHBCLKCTRL0 |= 1 << 20;  //abilito il GPIO1
   InitCapT();
   ACT_B

   while (1) {
      if (LPC_CAPT->TOUCH & 1 << 16) {
         B_ON;
      } else {
         B_OFF;
      }
   }
}
