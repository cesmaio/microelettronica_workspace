/****************************************************************************
*  LPC81x blinky demonstration program for LPC812 LPCXpresso board
*
*  1. Use SCT timer to generate a 10 msec timer tick (interrupt driven).
*  2. Toggle P0_7 (red LED) every 200 msec
*****************************************************************************/

#include "LPC8xx.h"
#include "sct_fsm.h"


void SCT_IRQHandler (void)
{                                                  /* SCT Interrupt Handler (10 msec) */
  static uint8_t cnt = 0;

    if (++cnt == 20)                                /* 20 * 10 msec = 200 msec       */
    {
        cnt = 0;
        LPC_GPIO_PORT->NOT0 = (1 << 7);            /* toggle pin P0_7                */
    }
    LPC_SCT->EVFLAG = 0x00000001;                  /* clear event 0 flag             */
}

int main (void) 
{
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8) | (1 << 6); /* enable clock to SCT and GPIO   */

    LPC_GPIO_PORT->DIR0       |= (1 << 7);          /* configure P0.7 (LED) as output  */

    sct_fsm_init();                                 /* Initialize SCT                  */
    NVIC_EnableIRQ(SCT_IRQn);                       /* enable SCT interrupt  */
    LPC_SCT->CTRL_U &= ~(1 << 2);                   /* unhalt the SCT by clearing bit 2 of the unified CTRL register  */

    while (1)                                       /* loop forever  */
    {
    }
}
