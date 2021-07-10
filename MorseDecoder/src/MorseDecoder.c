/*
=======================================================================================
 Name        : MorseDecoder.c
 Author      : Cesare Maio
 Version     : 1.0
 Copyright   : Cesare Maio
 Description : a Morse Code translator for LPC847 (go to /docs/README.txt for more info)
=======================================================================================
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

/* Follow the INTERNATIONAL MORSE CODE (check /docs/international_morse_code.png)*/
// 1. A dash is equal to 3 dots
// 2. The space between parts of the same letter is equal to 1 dot
// 3. The space between two letters is equal to 3 dots
// 4. The space between two words is equal to 5 dots
#define dot 1
#define dash 3
#define space 5
/* dichiaro le lettere (i simboli) traducibili dal codice morse */
unsigned int A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z;
unsigned int _1, _2, _3, _4, _5, _6, _7, _8, _9, _0;
#define EndOfLetter 9  // simboleggia la fine della lettera composta da simboli in codice morse

uint32_t prev_time = 0, dt = 0;  // per il calcolo dell'intervallo di tempo nel CTimer (in ms)
unsigned int measure = 0;        // indica che sta avvenendo una misura di tempo (per scrittura codice morse)

// # prototipi
unsigned int setupLCD(unsigned int bit);  // init LCD 2 lines 4 bit/8 bit
// per misurare l'intervallo temporale di pressione/rilascio (falling and rising edges) del pulsante di input (per la scrittura del codice morse)
void InitCTimer();  // inizializzo il CTimer per misurare l'intervallo di pressione del pulsante (rising and falling edges)
// per indicare all'utente il rate di lettura del codice morse (che si scrive utilizzando il pulsante di input) utilizzo SCT per comandare il led verde in PWM
void InitSCT(unsigned int SystemCoreClock, uint32_t timeUnit /* in ms */);  // inizializza SCT impostando il periodo in base all'unità temporale (in ms)
unsigned int setMorseRules();                                               // assegna i valori per la conversione alle lettere (simboli morse)
unsigned int translateMorseSymbol(uint32_t time, uint32_t timeUnit /* in ms*/);
unsigned int translateMorseLetter(unsigned int letter, int MAX_SIZE);

int main(void) {
   uint32_t timeUnit = 200;         // imposta l'unità temporale (rate di scrittura codice morse)
   unsigned int symbol;             // tradotto in codice morse dalla lettura
   unsigned int morseLetter[10];    // la "lettera" composta da simboli in codice morse (tratto/punto)
   unsigned int lastSymbolPos = 0;  // posizione dell'ultimo simbolo inserito in morseWord
   unsigned char phrase[42];        // la "frase" tradotta (non supera le dimensioni del display)

   InitSCT(SSC, timeUnit);  // Lampeggia GREEN LED in PWM per indicare l'unità temporale (di scrittura del codice morse)
   InitCTimer();            // cattura gli eventi di "press" and "release" del pulsante per scrivere il codice morse
   if (!setupLCD(8)) {
      R_ON return 1;
   }  // accendi LED R on board se c'è un errore
   setMorseRules();

   PutCommand(LINE1_HOME);

   while (1) {
      if (measure) {
         symbol = translateMorseSymbol(dt, timeUnit);
         if (!symbol) {                                     // errore di inserimento
         } else if (symbol == dash && lastSymbolPos % 2) {  // se il simbolo è in posizione dispari ed ha la lunghezza di un dash, è uno spazio tra due lettere
            morseLetter[lastSymbolPos] = EndOfLetter;
            translateMorseLetter(morseLetter, 10);
         } else {
            morseLetter[lastSymbolPos++] = symbol;
         }
         Write_ndigitsval(symbol, 1);

         measure = 0;  // reset misurazione in corso
      }

      // reset dei registri per evitare di superare 24bit
      if (LPC_CTIMER0->TC > 0xFFFFFFF) {
         LPC_CTIMER0->TCR = 1 << 1;
         LPC_CTIMER0->TCR = 1 << 0;  // riabilita il conteggio
      }
   }

}  // end of main

unsigned int setupLCD(unsigned int bit) {
   LPC_SYSCON->SYSAHBCLKCTRL0 |= 1 << 20;  // turn on GPIO1
   ACT_RGB

   if (bit == 4) {
      InitLCD_4b_2L();
   } else if (bit == 8) {
      InitLCD_8b_2L();
   } else {
      return 0;  // bit è sbagliato
   }
   PutCommand(DISP_ON_CUR_ON_BLINK_ON);  // no cursor, no blink
   return 1;
}

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
   if (LPC_CTIMER0->IR & (1 << 4)) {  // evento - record time in ms
      dt = LPC_CTIMER0->CR[0] - prev_time;
      prev_time = LPC_CTIMER0->CR[0];

      measure = 1;  // misurazione effettuata, riporta risultato nel main
      // elimino la pausa antirmbalzo per una misura più precisa
      LPC_CTIMER0->IR = 1 << 4;  // Bisogna eliminare l'interrupt
   }
}

void InitSCT(unsigned int SystemCoreClock, uint32_t timeUnit /* in ms*/) {
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

unsigned int setMorseRules() {
   /* Follow the INTERNATIONAL MORSE CODE (check /docs/international_morse_code.png)*/
   /* *
    * ogni lettera è associata ad un numero intero le cui cifre di posizione pari (partendo a contare da 0)
    * indicano un 'dot' o un 'dash'. Mentre le cifre di ordine dispari indicano lo spazio fra due simboli
    * (che ha le dimensioni di un dot)
    * */

   A = 113;
   B = 3111111;
   C = 3111311;
   D = 31111;
   E = 1;
   F = 1111311;
   G = 31311;
   H = 1111111;
}

unsigned int translateMorseSymbol(uint32_t time, uint32_t timeUnit /* in ms*/) {
   unsigned int symbol = time / timeUnit;  // interpreta il simbolo corrispondente alle nità temporali
                                           // per semplicità di inserimento sono stati scelti range molto ampi per l'interpretazione dei simboli
   if (symbol >= 0 && symbol < 2) {
      return dot;
   }
   if (symbol >= 2 && symbol < 4) {
      return dash;
   }
   if (symbol >= 4 && symbol < 6) {
      return space;
   }
   return 0;  // errore di inserimento

   // uint32_t modulo = time % timeUnit;
   // // get only first digit
   // while (modulo >= 10) {
   //    modulo = modulo / 10;
   // }
   // // approssima per eccesso o per difetto l'intervallo temporale letto per suddividerlo in unità temporali
   // unsigned int eccesso = 0;
   // if (modulo > 5) {
   //    eccesso = 1;
   // }

   // unsigned int symbol = 0;
   // switch ((time / timeUnit) + eccesso) {  // interpreta il simbolo corrispondente alle unità di tempo
   //    case 0:
   //       symbol = dot;  // tempo minimo viene comunque interpretato come punto
   //       break;
   //    case dot:
   //       symbol = dot;
   //       break;
   //    case dash:
   //       symbol = dash;
   //       break;
   //    case space:
   //       symbol = space;
   //       break;
   //    default:
   //       // in caso di errore scrivi '#'
   //       break;
   // }
   // return symbol;
}

unsigned int translateMorseLetter(unsigned int *letter, int MAX_SIZE) {
   int num = 0;
   for (unsigned int i = 0; i < MAX_SIZE; i++) {
      if (letter[i] == EndOfLetter) {
         break;
      }
      num = 10 * num + letter[i];  // converti array di numeri in singolo numero intero
   }
   return num;
}