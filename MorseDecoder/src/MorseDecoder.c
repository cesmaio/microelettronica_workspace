/*
===============================================================================
 Name        : .c
 Author      : Cesare Maio
 Version     : 1.0
 Copyright   : Cesare Maio
 Description : a reaction mini-game
===============================================================================
*/

#include "HD44780.h"
#include "LED.h"
#include "LPC8xx.h"
#include "inmux_trigmux.h"
#include "sct.h"
#include "swm.h"
#include "syscon.h"
#include "utilities.h"

// Macros to define the PWM period and the duty-cycles.
// This example implements 'edge-aligned' PWM.
// The SCT counter will count from 0 up to timeUnit, then start again at 0.
#define SSC 30207000  // SystemCoreClock (colpi di clock in 1s)

unsigned int measure = 0;
uint32_t prev_time = 0, delta_t = 0;

// prototipi
void InitCTimer();                                                              // inizializzo il CTimer per misurare l'intervallo di pressione del pulsante (rising and falling edges)
void InitSCT(unsigned int SystemCoreClock, unsigned int timeUnit /* in ms */);  // inizializza SCT impostando il periodo in base all'unità temporale (in ms)

int main(void) {
   InitSCT(SSC, 500);
   InitCTimer();
   InitLCD_4b_2L();

   // messaggio di inizio
   PutCommand(LINE1_HOME);
   WriteAfter((uint8_t*)" Premi USER btn ");

   while (1) {
      if (measure) {
         PutCommand(LINE2_HOME);
         Write_ndigitsval_space(delta_t / 1000, 6);
         WriteAfter((uint8_t*)".");
         Write_ndigitsval(delta_t % 1000, 3);
         WriteAfter((uint8_t*)" s   ");

         measure = 0;
      }

      // reset dei registri per evitare di superare 24bit
      if (LPC_CTIMER0->TC > 0xFFFFFFF) {
         LPC_CTIMER0->TCR = 1 << 1;
         LPC_CTIMER0->TCR = 1 << 0;  // riabilita il conteggio
      }
   }

}  // end of main

// inizializzo il CTimer per misurare l'intervallo di pressione del pulsante (rising and falling edges)
void InitCTimer() {
   LPC_SYSCON->SYSAHBCLKCTRL[0] |= (1 << 7) | (1 << 25);  // Accendi clock per SWM e CTIMER0

   // Configurazione del Counter/Timer Control Register
   // Counter mode: timer mode (clock della periferica)
   LPC_CTIMER0->CTCR = 0x0 << 0;  // come da reset

   LPC_CTIMER0->CCR = (1 << 1) |  //cattura TC su fronte di discesa di CAP0
                      (1 << 0) |  // cattura TC su fronte di salita di CAP0
                      (1 << 2);   //genera interrupt sull'evento di cattura da CAP0

   LPC_CTIMER0->TCR = 1 << 1;  // Reset dei registri

   NVIC_EnableIRQ(CTIMER0_IRQn);  // Abilita l'interrupt di CTIMER0 sulla CPU

   LPC_CTIMER0->PR = 30000000 / 1000 - 1;  // 1ms di periodo

   // config Switch Matrix
   // TO_CAP0 connesso a P0.8
   unsigned int dummyVal = LPC_SWM->PINASSIGN[14];
   dummyVal &= ~(0xFF << 8);  //azzera bit 8:15
   dummyVal |= (0x08 << 8);   //P0.8
   LPC_SWM->PINASSIGN[14] = dummyVal;

   LPC_CTIMER0->TCR = 1 << 0;  // Abilita CTIMER0
}

void CTIMER0_IRQHandler() {
   if (LPC_CTIMER0->IR & (1 << 4)) {
      delta_t = LPC_CTIMER0->CR[0] - prev_time;
      prev_time = LPC_CTIMER0->CR[0];

      measure = 1;
      // elimino la pausa antirmbalzo per una misura più precisa
      LPC_CTIMER0->IR = 1 << 4;  // Bisogna eliminare l'interrupt
   }
}

void InitSCT(unsigned int SystemCoreClock, unsigned int timeUnit /* in ms*/) {
   unsigned int basePeriod = SystemCoreClock / (1000 / timeUnit);  // lunghezza del periodo per SCT

   unsigned int GreenON = 0;                 // il LED VERDE si accende all'inizio del periodo (stato 0)
   unsigned int GreenOFF = basePeriod / 10;  // il LED VERDE si spegne poco dopo
   unsigned int RedON = 0;                   // il LED ROSSO si accende all'inizio del periodo (stato 1)
   unsigned int RedOFF = basePeriod / 10;    // il LED ROSSO si spegne poco dopo

   uint32_t temp;

   // Enable clocks to relevant peripherals: SWM e SCT and GPIO
   LPC_SYSCON->SYSAHBCLKCTRL0 |= (1 << 7) | (1 << 8) | (1 << 20);

   // Configuro Switch matrix
   // assegno P0_4 (tasto USER) a SCT0 con Switch Matrix
   temp = LPC_SWM->PINASSIGN[6];  //copio Pin assign register 6 (per SCT0_GPIO_IN_A_I)
   temp &= ~(0xFF << 24);         //azzero bit di selezione pin (31:24)
   temp |= (0x04 << 24);          //imposto P0_4 (tasto USER)
   LPC_SWM->PINASSIGN[6] = temp;  // imposto registro

   // assegno P1_0 (LED VERDE) a SCT_OUT0 con Switch Matrix
   temp = LPC_SWM->PINASSIGN[7];  //copio Pin assign register 6 (per SCT_OUT0_O)
   temp &= ~(0xFF << 24);         //azzero bit di selezione pin (31:24)
   temp |= (0x20 << 24);          //imposto P1_0 (LED VERDE)
   LPC_SWM->PINASSIGN[7] = temp;  // imposto registro

   // assegno P1_2 (LED ROSSO) a SCT_OUT1 con Switch Matrix
   // temp = LPC_SWM->PINASSIGN[8];  //copio Pin assign register 8 (per SCT_OUT1_O)
   // temp &= ~(0xFF << 24);         //azzero bit di selezione pin (31:24)
   // temp |= (0x22 << 24);          //imposto P1_0 (LED ROSSO)
   // LPC_SWM->PINASSIGN[8] = temp;  // imposto registro

   // Write to SCT0_INMUX[0] to select SCT_PIN0 function (which was connected to IN_PORT in the switch matrix) as SCT input SCT_IN0
   LPC_INMUX_TRIGMUX->SCT0_INMUX0 = 0x0;

   // Configure the SCT
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
   // No events serve as counter limits because we are using the AUTOLIMIT feature of match0 (see the CONFIG reg. config.)
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
   // Simultaneous set and clear (which would be a programming error, and won't happen here) has no effect for all outputs
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
   LPC_SCT->MATCH[0].U = basePeriod;
   LPC_SCT->MATCHREL[0].U = basePeriod;
   LPC_SCT->MATCH[1].U = GreenON;
   LPC_SCT->MATCHREL[1].U = GreenON;
   LPC_SCT->MATCH[2].U = GreenOFF;
   LPC_SCT->MATCHREL[2].U = GreenOFF;
   LPC_SCT->MATCH[3].U = RedON;
   LPC_SCT->MATCHREL[3].U = RedON;
   LPC_SCT->MATCH[4].U = RedOFF;
   LPC_SCT->MATCHREL[4].U = RedOFF;

   // Configure the EVENT_STATE and EVENT_CTRL registers for all events
   //
   // Event EVENT_STATE                        EVENT_CTRL
   // ----- ---------------------------------  -------------------------------------------------------------------------------------------
   // EV0   Enabled in State 0 (blink_green).  A match in Match1 is associated with this event (green on), no effect on state.
   // EV1   Enabled in State 0 (blink_green).  A match in Match2 is associated with this event (green off), no effect on state.
   // EV2   Enabled in State 0 (blink_green).  A match in Match0 AND CTIN_0 low is associated with this event, changes state from 0 to 1.
   // EV3   Enabled in State 1 (blink_red).    A match in Match3 is associated with this event (red on), no effect on state.
   // EV4   Enabled in State 1 (blink_red).    A match in Match4 is associated with this event (red off), no effect on state.
   // EV5   Enabled in State 1 (blink red).    A match in Match0 AND CTIN_0 high is associated with this event, changes state from 1 to 0.
   LPC_SCT->EVENT[0].STATE = 1 << 0;  // Event0 is enabled in state 0
   LPC_SCT->EVENT[1].STATE = 1 << 0;  // Event1 is enabled in state 0
   LPC_SCT->EVENT[2].STATE = 1 << 0;  // Event2 is enabled in state 0
   LPC_SCT->EVENT[3].STATE = 1 << 1;  // Event3 is enabled in state 1
   LPC_SCT->EVENT[4].STATE = 1 << 1;  // Event4 is enabled in state 1
   LPC_SCT->EVENT[5].STATE = 1 << 1;  // Event5 is enabled in state 1
   LPC_SCT->EVENT[0].CTRL = (0x1 << MATCHSEL) | (Match_Only << COMBMODE) | (0 << STATELD) | (0 << STATEV);
   LPC_SCT->EVENT[1].CTRL = (0x2 << MATCHSEL) | (Match_Only << COMBMODE) | (0 << STATELD) | (0 << STATEV);
   LPC_SCT->EVENT[2].CTRL = (0x0 << MATCHSEL) | (0 << OUTSEL) | (0 << IOSEL) | (LOW << IOCOND) | (Match_and_IO << COMBMODE) | (1 << STATELD) | (1 << STATEV);  //va in stato 1
   LPC_SCT->EVENT[3].CTRL = (0x3 << MATCHSEL) | (Match_Only << COMBMODE) | (0 << STATELD) | (0 << STATEV);
   LPC_SCT->EVENT[4].CTRL = (0x4 << MATCHSEL) | (Match_Only << COMBMODE) | (0 << STATELD) | (0 << STATEV);
   LPC_SCT->EVENT[5].CTRL = (0x0 << MATCHSEL) | (0 << OUTSEL) | (0 << IOSEL) | (HIGH << IOCOND) | (Match_and_IO << COMBMODE) | (1 << STATELD) | (0 << STATEV);  //torna in stato 0

   // Configure the OUT registers for the 4 SCT outputs
   // Event 0 clears output CTOUT_0 to '0'   (green LED on)
   // Event 1 sets output CTOUT_0 to '1' (green LED off)
   // Event 3 clears output CTOUT_1 to '0'   (red LED on)
   // Event 4 sets output CTOUT_1 to '1' (red LED off)
   LPC_SCT->OUT[0].SET = 1 << event1;
   LPC_SCT->OUT[0].CLR = 1 << event0;
   LPC_SCT->OUT[0].SET |= 1 << event4;
   LPC_SCT->OUT[0].CLR |= 1 << event3;
   LPC_SCT->OUT[3].SET = 0;  // Unused output, set by no event (default)
   LPC_SCT->OUT[3].CLR = 0;  // Unused output, cleared by no event (default)

   // FINALLY ... now let's run it. Clearing bit 2 of the CTRL register takes it out of HALT.
   LPC_SCT->CTRL_L &= ~(1 << Halt_L);
}