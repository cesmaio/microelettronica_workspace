/*
===============================================================================
 Name        : SCT_PPMperSEC
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include "Delay.h"
#include "HD44780.h"
#include "LED.h"
#include "LPC8xx.h"
#include "adc.h"

// Pins used in this example:
// green led port pin [Output] per segnalare l'accensione
// ADC1: lettura tensione di ingresso
// P0.11: uscita PPM
// P0.0: switch saldato sulla millefori

/*
 * Ho disabilitato la funzione che pone il segnale solo quando si preme il
 * pulsante collegato a P0.0
 */

// Macros to define the PWM period and the duty-cycles.
// This example implements 'edge-aligned' PWM.
// The SCT counter will count from 0 up to pwm_period, then start again at 0.
//#define PPM_PERIOD     30000000/1000*20 // 20 ms
// il periodo misurato per 600000 colpi di clock è di 19.863 ms.
// Per cui:
//			20 ms : 604138 colpi di clock
//			 1 ms :  30207 colpi di clock
//			 2 ms :  60414 colpi di clock
#define PWM_PERIOD 604138       // 20 ms
#define C_match_ON 0            // il LED verde si accende all'inizio del periodo (stato 0)
#define C_match_OFF_MAX 604130  // 30000000/1000*2 // 2 ms: valore max per cui il LED VERDE si spegne
#define C_match_OFF_MIN 30207   // 30000000/1000*1 // 1 ms: valore max per cui il LED VERDE si spegne
#define ONE_ms 30207            // Attenzione che 1 ms non è 30000 ma 30200 e passa...

#define PIN_USCITA (1 << 4)   // l'uscita PPM è tasto user
#define PIN_SWITCH (1 << 16)  // lo switch lo simuliamo con il primo pin della board
#define CHANNEL 8             // canale usato per l'ADC PIN 18
#define READINGS 8            // numero di letture

int main(void) {
   uint32_t nTck = C_match_OFF_MIN;
   uint32_t switch_pressed = 0;
   uint32_t voltage, permille, dummyVal;
   uint8_t stato = 0;
   uint8_t i, escape = 0;

   // 00. Abilita clock per GPIO1 (led verde)
   LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1 << 20 | 1 << 25;  // Turn on clock to GPIO_01

   // 0.1 lampeggio led verde per segnalare accensione
   ACT_G
   for (i = 0; i < 3; i++) {
      LED_G DelayMs(250);
      G_OFF DelayMs(250);
   }
   LED_G
   InitLCD_4b_2L();  //inizializza LCD

   InitADC(CHANNEL);  // su ADC0 c'è il trimmer della LPC845-BRK
                      // INVECE la tensione di ingresso arriva su ADC1

   InitSCT();

   // FINALLY ... now let's run it. Clearing bit 2 of the CTRL register takes it out of HALT.
   LPC_SCT->CTRL_L &= ~(1 << 2);  // HALT_L = 1

   WriteInitial((uint8_t*)"Ciao Francesco  ");
   DelayMs(400);
   for (i = 0; i < 4; i++) {
      PutCommand(DISPLAY_MOVE_SHIFT_LEFT);
      DelayMs(150);
   }
   DelayMs(1000);
   PutCommand(ENTRY_MODE_INC_NO_SHIFT);

   while (1) {
      dummyVal = ReadADC(CHANNEL, READINGS);
      if (dummyVal < 11)
         voltage = 0;
      else
         voltage = (332000 * dummyVal / 4096 + 5) / 10;  // tensione di ingresso in mV

      permille = (10000 * dummyVal / 4096 + 5) / 10;

      nTck = (C_match_OFF_MIN + permille * (C_match_OFF_MAX - C_match_OFF_MIN) / 1000);  // in periodi di clock
      LPC_SCT->MATCHREL[2].U = nTck;                                                     // setta il valore di match per porre basso il segnale di uscita
          // cambiando il valore di match reload, al periodo successivo
          // si ha la nuova impostazione di match.
          // il [2] è il match che porta l'uscita alta (led spento)
      PutCommand(RETURN_HOME);
      DelayMs(5);

      if (((LPC_GPIO_PORT->PIN[0] & (1 << 6)) == 0) && ((switch_pressed & (1 << 6)) == 0))  //tasto 0 premuto
      {                                                                                     //per vedere i diversi valori
         switch_pressed |= 1 << 6;
         DelayMs(50);
         stato++;
         if (stato > 4) stato = 0;
         PutCommand(LINE2_HOME);
         switch (stato) {
            case 0:  // visualizza percentuale
               WriteAscii(' ');
               Write_ndigitsval_space(permille / 10, 3);
               WriteAscii('.');
               Write_ndigitsval_space(permille % 10, 1);
               WriteAfter((uint8_t*)"%   ");
               break;

            case 1:  // visualizza periodo
               WriteAscii(' ');
               Write_ndigitsval_space(nTck * 1000 / ONE_ms, 5);
               WriteAfter((uint8_t*)" us ");
               break;

            case 2:  // visualizza mV
               Write_ndigitsval_space(voltage / 10, 4);
               WriteAscii('.');
               Write_ndigitsval_space(voltage % 10, 1);
               WriteAfter((uint8_t*)" mV ");
               break;

            case 3:  // visualizza N_ADC
               WriteAfter((uint8_t*)"N: ");
               Write_ndigitsval_space(dummyVal, 5);
               WriteAfter((uint8_t*)"  ");
               break;

            case 4:  // visualizza numero di periodi di clock
               Write_ndigitsval_space(nTck, 6);
               WriteAfter((uint8_t*)" Tck");
               break;
         }
      }

      if (((LPC_GPIO_PORT->PIN[0] & (1 << 6))) && (switch_pressed & (1 << 6)))  //tasto 0 rilasciato
      {
         switch_pressed &= ~(1 << 6);
         DelayMs(25);
      }

   }  // end of while(1)

}  // end of main

// INIZIALIZZA SCT PER CREARE PPM
void InitSCT(void) {
   uint32_t temp;

   // Enable clocks to relevant peripherals: SWM e SCT
   LPC_SYSCON->SYSAHBCLKCTRL0 |= (1 << 7) | (1 << 8);

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
