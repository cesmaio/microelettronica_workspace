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

void InitSCT(void) {
   uint32_t temp;

   // Enable clocks to relevant peripherals: SWM (switch matrix) e SCTimer
   LPC_SYSCON->SYSAHBCLKCTRL0 |= (1 << 7) | (1 << 8);

   // INPUT per SCT

   // Configurazione SWM
   // assegno P1_0 (LED VERDE) a SCT_OUT0 con Switch Matrix [SCT output 0 on green LED port pin (0=on, 1=off)]
   temp = LPC_SWM->PINASSIGN[7];  //copio Pin assign register 6 (per SCT_OUT0_O)
   temp &= ~(0xFF << 24);         //azzero bit di selezione pin (31:24)
   temp |= (0x20 << 24);          //imposto P1.0
   LPC_SWM->PINASSIGN[7] = temp;  // imposto registro

   // CONFIGURE the SCT ...
   // Give the module a reset
   LPC_SYSCON->PRESETCTRL[0] &= ~(1 << 8);  //Assert the SCT reset
   LPC_SYSCON->PRESETCTRL[0] |= (1 << 8);   //Clear the SCT reset

   // Configure the CONFIG register
   // CKSEL=unused(default), NORELOADL/H=unused(default), INSYNC=unused(default),
   LPC_SCT->CONFIG |= (1 << 0) |  // UNIFY counter
                      (0 << 1) |  // CLKMODE=busclock
                      (1 << 17);  // AUTOLIMIT=true

   // Configure the CTRL register
   // Don't run yet, clear the counter, BIDIR=0(default,unidirectional up-count), PRE=0(default,div-by-1)
   LPC_SCT->CTRL |= (0 << 1) |  // STOP_L:   Stay in halt mode until SCT setup is complete
                    (1 << 2) |  // HALT_L:   Stay in halt mode until SCT setup is complete
                    (1 << 3) |  // CLRCTR_L: Clear the counter (good practice)
                    (0 << 4) |  // BIDIR_L:  Unidirectional mode (Up-count)
                    (0 << 5);   // PRE_L:    Prescaler = div-by-1

   // Setup the LIMIT register
   // No events serve as counter limits because we are using the
   // AUTOLIMIT feature of match0 (see the CONFIG reg. config.)
   LPC_SCT->LIMIT_L = 0;

   // Setup the HALT register
   // No events will set the HALT_L bit in the CTRL reg.
   LPC_SCT->HALT_L = 0;

   // Setup the STOP register
   // No events will set the STOP_L bit in the CTRL reg.
   LPC_SCT->STOP_L = 0;

   // Setup the START register
   // No events will clear the STOP_L bit in the CTRL reg.
   LPC_SCT->START_L = 0;

   // Initialize the COUNT register
   // Start counting at '0'
   LPC_SCT->COUNT = 0;

   // Initialize the STATE register
   // Start in state 0
   LPC_SCT->STATE_L = 0;

   // Setup the REGMODE register
   // All Match/Capture registers act as match registers
   LPC_SCT->REGMODE_L = 0;

   // Configure the OUTPUT register
   // Initialize CTOUT_0 to '1' so the LED is off to begin with
   LPC_SCT->OUTPUT = (1 << 0);

   // Configure the OUTPUTDIRCTRL register
   // The counting direction has no impact on the meaning of set and clear for all outputs
   LPC_SCT->OUTPUTDIRCTRL = 0;

   // Configure the RES register
   // Simultaneous set and clear (which would be a programming error,
   // and won't happen here) has no effect for all outputs
   LPC_SCT->RES = 0;

   // Configure the EVEN register
   // This example does not use interrupts, so don't enable any event flags to interrupt.
   LPC_SCT->EVEN = 0;

   // Clear any pending event flags by writing '1's to the EVFLAG register
   LPC_SCT->EVFLAG = 0xFF;

   // Configure the CONEN register
   // This example does not use interrupts, so don't enable any 'no-change conflict' event flags to interrupt.
   LPC_SCT->CONEN = 0;

   // Clear any pending 'no-change conflict' event flags, and BUSSERR flags, by writing '1's to the CONLAG register
   LPC_SCT->CONFLAG = 0xFFFFFFFF;

   // Configure the match registers (and their associated match reload registers, which will be the same for this example)
   // for the PWM duty cycles desired
   LPC_SCT->MATCH[0].U = PWM_PERIOD;          // Match0 is the AUTOLIMIT event, determines the period of the PWM
   LPC_SCT->MATCHREL[0].U = PWM_PERIOD;       // "
   LPC_SCT->MATCH[1].U = C_match_ON;          // CTOUT_0 = '0'
   LPC_SCT->MATCHREL[1].U = C_match_ON;       // "
   LPC_SCT->MATCH[2].U = C_match_OFF_MAX;     // CTOUT_0 = '1'
   LPC_SCT->MATCHREL[2].U = C_match_OFF_MAX;  // "

   // Configure the EVENT_STATE and EVENT_CTRL registers for all events
   //
   // Event EVENT_STATE                        EVENT_CTRL
   // ----- ---------------------------------  -------------------------------------------------------------------------------------------
   // EV0   Enabled in State 0 (blink_green).  A match in Match1 is associated with this event (green on), no effect on state.
   // EV1   Enabled in State 0 (blink_green).  A match in Match2 is associated with this event (green off), no effect on state.

   LPC_SCT->EVENT[0].STATE = 1 << 0;  // Event0 is enabled in state 0
   LPC_SCT->EVENT[1].STATE = 1 << 0;  // Event1 is enabled in state 0

   LPC_SCT->EVENT[0].CTRL = (0x1 << 0)    // MATCHSEL = 1
                            | (1 << 12)   //COMBMODE = 1 (Match Only)
                            | (0 << 14)   //STATELD = 0
                            | (0 << 15);  //STATEV = 0
   LPC_SCT->EVENT[1].CTRL = (0x2 << 0)    // MATCHSEL = 2
                            | (1 << 12)   //COMBMODE = 1 (Match Only)
                            | (0 << 14)   //STATELD = 0
                            | (0 << 15);  //STATEV = 0

   // Configure the OUT registers for the 4 SCT outputs
   // Event 0 clears output CTOUT_0 to '0'   (green LED on)
   // Event 1 sets output CTOUT_0 to '1' (green LED off)
   LPC_SCT->OUT[0].SET = 1 << 1;  // evento 1
   LPC_SCT->OUT[0].CLR = 1 << 0;  // evento 0
                                  //  LPC_SCT->OUT[0].SET |= 1<<event4; (si possono aggiungere eventi per la stessa uscita)
                                  //  LPC_SCT->OUT[0].CLR |= 1<<event3;
   LPC_SCT->OUT[3].SET = 0;       // Unused output, set by no event (default)
   LPC_SCT->OUT[3].CLR = 0;       // Unused output, cleared by no event (default)
}