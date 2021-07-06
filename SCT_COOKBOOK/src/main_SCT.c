/*
===============================================================================
 Name        : SCT_PWM_2State_main.c
 Author      : Andrea
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
Configuration
 Input(s) used: SCT_IN0 (apply the 10 kHz PWM signal here)
 Output(s) used:
 SCT_OUT0, timeout indicator, low active. Output timeout activated if no edge is detected for three PWM periods.
 SCT_OUT1, indicator for duty cycle out of bounds, low active. This output is also active when a timeout occurs.
 Match/Cap used: Match 0 to 2 and Capture 3 and 4.
 Event used: Event 0 to 5.
 State used: State 0 and 1.
*/

#include "LED.h"
#include "LPC8xx.h"
#include "inmux_trigmux.h"
#include "sct.h"
#include "swm.h"
#include "syscon.h"

#define SCC 30207000  //SystemCoreClock

extern volatile uint8_t ev0_handshake, /*ev1_handshake,*/ interrupt_counter;

// Main da modificare a seconda della funzione di Init da implementare
int main(void) {
   ACT_R
   // Enable clocks to relevant peripherals: SWM e SCT
   LPC_SYSCON->SYSAHBCLKCTRL0 |= (1 << 7) | (1 << 8);
   // configure the SWM to output CTOUT_0 to PIO0_7 //
   LPC_SWM->PINASSIGN6 = 0x07ffffff;
   // Give the module a reset
   LPC_SYSCON->PRESETCTRL[0] &= ~(1 << 8);  //Assert the SCT reset
   LPC_SYSCON->PRESETCTRL[0] |= (1 << 8);   //Clear the SCT reset

   void SCT_Init2(void);          // ## Initialize the SCT
   LPC_SCT->CTRL_L &= ~(1 << 2);  // Unhalt the SCT by clearing bit 2 of the CTRL register //

   // Enter an infinite loop, just incrementing a counter //
   volatile static int i = 0;

   while (1) {
      i++;
   }
   return 0;
}

void SCT_Init1(void)  //repetitive IRQ
{
   LPC_SCT->CONFIG = (1 << 0) | (1 << 17);  // unified 32-bit timer, auto limit
   LPC_SCT->MATCH[0].U = SCC / 100;         // match 0 @ 100 Hz = 10 msec
   LPC_SCT->MATCHREL[0].U = SCC / 100;      // match 0 @ 100 Hz = 10 msec
   LPC_SCT->EVENT[0].STATE = 0xFF;          // event 0 happens in all states
   LPC_SCT->EVENT[0].CTRL = (1 << 12);      // match 0 condition only
   LPC_SCT->EVEN = (1 << 0);                // event 0 generates an interrupt
   NVIC_EnableIRQ(SCT_IRQn);                // enable SCTimer/PWM interrupt
}

void SCT_Init2(void)  // Blinky Match
{
   LPC_SCT->CONFIG |= 1;                     // unified timer
   LPC_SCT->MATCHREL[0].U = (SCC / 10) - 1;  // match 0 @ 10 Hz = 100 msec
   LPC_SCT->EVENT[0].STATE = (1 << 0);       // event 0 only happens in state 0
   LPC_SCT->EVENT[0].CTRL = (0 << 0) |       // related to match 0
                            (1 << 12) |      // COMBMODE[13:12] = match condition only
                            (1 << 14) |      // STATELD[14] = STATEV is loaded into state
                            (1 << 15);       // STATEV[15] = 1 (new state is 1)
   LPC_SCT->EVENT[1].STATE = (1 << 1);       // event 1 only happens in state 1
   LPC_SCT->EVENT[1].CTRL = (0 << 0) |       // related to match 0
                            (1 << 12) |      // COMBMODE[13:12] = match condition only
                            (1 << 14) |      // STATELD[14] = STATEV is loaded into state
                            (0 << 15);       // STATEV[15] = 0 (new state is 0)
   LPC_SCT->OUT[0].SET = (1 << 0);           // event 0 will set SCT_OUT0
   LPC_SCT->OUT[0].CLR = (1 << 1);           // event 1 will clear SCT_OUT0
   LPC_SCT->LIMIT_L = 0x0003;                // events 0 and 1 are used as counter limit
}

void SCT_Init3(void)  //  Match & Toggle: toggle SCTx_OUT0 every 100 milliseconds
{
   LPC_SCT->CONFIG |= (1 << 17);                // two 16 bit timers, auto limit
   LPC_SCT->CTRL_L |= (199 << 5);               // PRE_L[12:5] = 200-1 (SCTimer/PWM clock = 30MHz/200 = 150 kHz)
   LPC_SCT->CTRL = 1 << Halt_H;                 // Si aggiunge lo STOP del Counter Alto per Risparmio energetico
   LPC_SCT->MATCHREL[0].L = (150000 / 15) - 1;  // match 0 @ 15 Hz = 100 msec
   LPC_SCT->EVENT[0].STATE = 0xFF;              // event 0 happens in all state
   LPC_SCT->EVENT[0].CTRL = (1 << 12);          // match 0 condition only
   LPC_SCT->OUT[0].SET = (1 << 0);              // event 0 will set SCTx_OUT0
   LPC_SCT->OUT[0].CLR = (1 << 0);              // event 0 will clear SCTx_OUT0
   LPC_SCT->RES = (3 << 0);                     // output 0 toggles on conflict
}

void SCT_Init4(void)  // Simple PWM
{
   LPC_SCT->CONFIG |= (1 << 17);                   // two 16-bit timers, auto limit
   LPC_SCT->CTRL_L |= (3 - 1) << 5;                // set prescaler, SCTimer/PWM clock = 10 MHz
   LPC_SCT->MATCHREL[0].L = 100 - 1;               // match 0 @ 100/10MHz = 10 usec (100 kHz PWM freq)
   LPC_SCT->MATCHREL[1].L = 50;                    // match 1 used for duty cycle (in 10 steps)
   LPC_SCT->EVENT[0].STATE = 0xFF;                 // event 0 happens in all states
   LPC_SCT->EVENT[0].CTRL = (1 << 12);             // match 0 condition only
   LPC_SCT->EVENT[1].STATE = 0xFF;                 // event 1 happens in all states
   LPC_SCT->EVENT[1].CTRL = (1 << 0) | (1 << 12);  // match 1 condition only
   LPC_SCT->OUT[0].SET = (1 << 0);                 // event 0 will set SCTx_OUT0
   LPC_SCT->OUT[0].CLR = (1 << 1);                 // event 1 will clear SCTx_OUT0
}

void SCT_Init5(void)  // Center Aligned PWM
{
   LPC_SCT->CONFIG |= (1 << 17);                     // two 16-bit timers, auto limit at match 0
   LPC_SCT->CTRL_L |= (1 << 4) | (15 - 1) << PRE_L;  // BIDIR mode, prescaler = 15, SCTimer/PWM clock = 2 MHz
   LPC_SCT->MATCHREL[0].L = 10 - 1;                  // match 0 @ 10/2MHz = 5 usec (100 kHz PWM freq)
   LPC_SCT->MATCHREL[1].L = 5;                       // match 1 used for duty cycle (in 10 steps)
   LPC_SCT->EVENT[0].STATE = 0xFF;                   // event 0 happens in all states
   LPC_SCT->EVENT[0].CTRL = (1 << 0) | (1 << 12);    // match 1 condition only
   LPC_SCT->OUT[0].SET = (1 << 0);                   // event 0 will set SCTx_OUT0
   LPC_SCT->OUTPUTDIRCTRL = (1 << 0);                // reverse output 0 set when down counting
}

void SCT_Init6(void)  // PWM with deadtime using no states, four events and three match / match reload registers.
{
#define DC1 (3020)        // duty cycle 1 messa a 100 usec
#define DC2 (4530)        // duty cycle 2 messa a 150 usec
#define hperiod (167816)  // periodo di 180 Hz

   LPC_SCT->CONFIG |= 1 | (1 << 17);  // unify timers, auto limit
   LPC_SCT->CTRL_L |= (1 << 4);       // configure SCT1 as BIDIR
   LPC_SCT->MATCH[0].U = hperiod;     // match on (half) PWM period
   LPC_SCT->MATCHREL[0].U = hperiod;
   LPC_SCT->MATCH[1].L = DC1;  // match on duty cycle 1
   LPC_SCT->MATCHREL[1].L = DC1;
   LPC_SCT->MATCH[2].L = DC2;  // match on duty cycle 2
   LPC_SCT->MATCHREL[2].L = DC2;
   LPC_SCT->EVENT[0].STATE = 0xFF;                  // event 0 happens in all states
   LPC_SCT->EVENT[0].CTRL = (2 << 10) | (2 << 12);  // IN_0 falling edge only condition
   LPC_SCT->EVENT[1].STATE = 0xFF;                  // event 1 happens in all states
   LPC_SCT->EVENT[1].CTRL = (1 << 10) | (2 << 12);  // IN_0 rising edge only condition
   LPC_SCT->EVENT[2].STATE = 0xFF;                  // event 2 happens in all states
   LPC_SCT->EVENT[2].CTRL = (1 << 0) | (1 << 12);   // match 1 (DC1) only condition
   LPC_SCT->EVENT[3].STATE = 0xFF;                  // event 3 happens in all states
   LPC_SCT->EVENT[3].CTRL = (2 << 0) | (1 << 12);   // match 2 (DC) only condition
   LPC_SCT->OUT[0].SET = (1 << 0) | (1 << 2);       // event 0 and 2 set OUT0 (blue LED)
   LPC_SCT->OUT[0].CLR = (1 << 2);                  // event 2 clears OUT0 (blue LED)
   LPC_SCT->OUT[1].SET = (1 << 3);                  // event 3 sets OUT1 (red LED)
   LPC_SCT->OUT[1].CLR = (1 << 0) | (1 << 3);       // event 0 and 3 clear OUT1 (red LED)
   // LPC_SCT->OUTPUTDIRCTRL = 0x5 ; // reverse output 0&1 set when down counting
   LPC_SCT->RES |= 0x0000000F;  // toggle OUT0 and OUT1 on conflict
   LPC_SCT->OUTPUT |= 1;        // default set OUT0 and clear OUT1
   LPC_SCT->STOP_L = (1 << 0);  // event 0 will stop the timer
   LPC_SCT->EVEN = (1 << 1);    // event 1 will generate an irq
   NVIC_EnableIRQ(SCT_IRQn);    // enable SCTx interrupt
}

/*void SCT_IRQHandler(void) // da usare per cambiare il MATCH del STC
{
	LPC_SCT->CONFIG |= (1 << 7); // stop reload process for L counter
	if (~(LPC_GPIO_PORT->PIN0 & (1 << 16))) // P0_16 low?
	{
		if (LPC_SCT->MATCHREL[2].L < hperiod-1) // check if DC2 < Period of PWM
		{
			if (LPC_SCT->MATCHREL[1].L > 1) {
			LPC_SCT->MATCHREL[1].L ++;
			LPC_SCT->MATCHREL[2].L ++;  }
		}
	}
	else if  (~(LPC_GPIO_PORT->PIN0 & (1 << 16))) // P0_17 low?
	{
		LPC_SCT->MATCHREL[1].L --;
		LPC_SCT->MATCHREL[2].L --;
	}
	LPC_SCT->CONFIG &= ~(1 << 7); // enable reload process for L counter
}*/

void SCT_Init7(void)  // Two-Channel PWM
{
#define delay (400000)            // duty cycle PWM1
#define match_green_OFF (500000)  // duty cycle PWM2
#define match_green_ON (100000)   // duty cycle PWM3
#define match_red_OFF (900000)    // duty cycle PWM4
#define match_red_ON (1000000)

   LPC_SCT->CONFIG |= (1 << 0) | (1 << 17);               // unified, auto limit
   LPC_SCT->MATCHREL[0].U = delay;                        // match_cycle
   LPC_SCT->MATCHREL[1].U = match_green_OFF;              // match_green_OFF
   LPC_SCT->MATCHREL[2].U = match_green_ON;               // match_green_ON
   LPC_SCT->MATCHREL[3].U = match_red_OFF;                // match_red_OFF
   LPC_SCT->MATCHREL[4].U = match_red_ON;                 // match_red_ON
   LPC_SCT->EVENT[0].STATE = (1 << 0);                    // event 0 happens in state 0 (U_ENTRY)
   LPC_SCT->EVENT[0].CTRL = (0 << 0) |                    // related to match_cycle
                            (0 << 10) |                   // IN_0 low
                            (3 << 12) |                   // match AND IO condition
                            (1 << 14) |                   // STATEV is loaded into state
                            (1 << 15);                    // new state is 1
   LPC_SCT->EVENT[1].STATE = (1 << 0);                    // event 1 happens in state 0 (U_ENTRY)
   LPC_SCT->EVENT[1].CTRL = (3 << 0) | (1 << 12);         // match_red_OFF only condition
   LPC_SCT->EVENT[2].STATE = (1 << 0);                    // event 2 happens in state 0 (U_ENTRY)
   LPC_SCT->EVENT[2].CTRL = (4 << 0) | (1 << 12);         // match_red_ON only condition
   LPC_SCT->EVENT[3].STATE = (1 << 1);                    // event 3 happens in state 1
   LPC_SCT->EVENT[3].CTRL = (0 << 0) |                    // related to match_cycle
                            (3 << 10) |                   // IN_0 high
                            (3 << 12) |                   // match AND IO condition
                            (1 << 14) |                   // STATEV is loaded into state
                            (0 << 15);                    // new state is 0
   LPC_SCT->EVENT[4].STATE = (1 << 1);                    // event 4 happens in state 1
   LPC_SCT->EVENT[4].CTRL = (2 << 0) | (1 << 12);         // match_green_ON only condition
   LPC_SCT->EVENT[5].STATE = (1 << 1);                    // event 5 happens in state 1
   LPC_SCT->EVENT[5].CTRL = (1 << 0) | (1 << 12);         // match_green_OFF only condition
   LPC_SCT->OUT[0].SET = (1 << 0) | (1 << 3) | (1 << 5);  // event 0, 3 and 5 set OUT0 (green LED)
   LPC_SCT->OUT[0].CLR = (1 << 4);                        // event 4 clear OUT0 (green LED)
   LPC_SCT->OUT[1].SET = (1 << 0) | (1 << 1) | (1 << 3);  // event 0, 1 and 3 set OUT1 (red LED)
   LPC_SCT->OUT[1].CLR = (1 << 2);                        // event 2 clear OUT1 (red LED)
   LPC_SCT->OUTPUT |= 3;                                  // default set OUT0 and OUT1
}

void SCT_Init8(void)  // Four-Channel PWM
{
#define pwm_val1 (400000)  // duty cycle PWM1
#define pwm_val2 (500000)  // duty cycle PWM2
#define pwm_val3 (100000)  // duty cycle PWM3
#define pwm_val4 (900000)  // duty cycle PWM4
#define pwm_cycle (1000000)

   LPC_SCT->CONFIG |= (1 << 0) | (1 << 17);  // unified timer, auto limit
   LPC_SCT->MATCH[0].U = pwm_cycle;          // match 0 on PWM cycle
   LPC_SCT->MATCHREL[0].U = pwm_cycle;
   LPC_SCT->MATCH[1].U = pwm_val1;  // match 1 on val1 (PWM1)
   LPC_SCT->MATCHREL[1].U = pwm_val1;
   LPC_SCT->MATCH[2].U = pwm_val2;  // match 2 on val2 (PWM2)
   LPC_SCT->MATCHREL[2].U = pwm_val2;
   LPC_SCT->MATCH[3].U = pwm_val3;  // match 3 on val3 (PWM3)
   LPC_SCT->MATCHREL[3].U = pwm_val3;
   LPC_SCT->MATCH[4].U = pwm_val4;  // match 4 on val4 (PWM4)
   LPC_SCT->MATCHREL[4].U = pwm_val4;
   LPC_SCT->EVENT[0].STATE = 0xFF;                  // event 0 happens in all states
   LPC_SCT->EVENT[0].CTRL = (0 << 0) | (1 << 12);   // match 0 (pwm_cycle) only condition
   LPC_SCT->EVENT[1].STATE = 0xFF;                  // event 1 happens in all states
   LPC_SCT->EVENT[1].CTRL = (1 << 0) | (1 << 12);   // match 1 (pwm_val1) only condition
   LPC_SCT->EVENT[2].STATE = 0xFF;                  // event 2 happens in all states
   LPC_SCT->EVENT[2].CTRL = (2 << 0) | (1 << 12);   // match 2 (pwm_val2) only condition
   LPC_SCT->EVENT[3].STATE = 0xFF;                  // event 3 happens in all states
   LPC_SCT->EVENT[3].CTRL = (3 << 0) | (1 << 12);   // match 3 (pwm_val3) only condition
   LPC_SCT->EVENT[4].STATE = 0xFF;                  // event 4 happens in all states
   LPC_SCT->EVENT[4].CTRL = (4 << 0) | (1 << 12);   // match 4 (pwm_val4) only condition
   LPC_SCT->EVENT[5].STATE = 0xFF;                  // event 5 happens in all states
   LPC_SCT->EVENT[5].CTRL = (0 << 10) | (2 << 12);  // IN_0 LOW only condition
   LPC_SCT->OUT[0].SET = (1 << 0);                  // event 0 sets OUT0 (PWM1)
   LPC_SCT->OUT[0].CLR = (1 << 1) | (1 << 5);       // event 1 and 5 clear OUT0 (PWM1)
   LPC_SCT->OUT[1].SET = (1 << 0);                  // event 0 sets OUT1 (PWM2)
   LPC_SCT->OUT[1].CLR = (1 << 2) | (1 << 5);       // event 2 and 5 clear OUT1 (PWM2)
   LPC_SCT->OUT[2].SET = (1 << 3) | (1 << 5);       // event 3 and 5 set OUT2 (PWM3)
   LPC_SCT->OUT[2].CLR = (1 << 0);                  // event 0 clear OUT2 (PWM3)
   LPC_SCT->OUT[3].SET = (1 << 4) | (1 << 5);       // event 4 and 5 set OUT3 (PWM4)
   LPC_SCT->OUT[3].CLR = (1 << 0);                  // event 0 clear OUT3 (PWM4)
   LPC_SCT->OUTPUT = 0x0000000C;                    // default clear OUT0/1 and set OUT2/3
   LPC_SCT->RES = 0x0000005A;                       // conflict: Inactive state takes precedence
   // SCT2_OUT0/1: Inactive state low
   // SCT2_OUT2/3: Inactive state high
   LPC_SCT->HALT_L = (1 << 5);           // event 5 will halt the timer
   LPC_SCT->LIMIT_L = (1 << 5);          // event 5 will limit the timer
   LPC_SCT->EVEN = (1 << 0) | (1 << 5);  // event 0 and 5 will generate an irq
   NVIC_EnableIRQ(SCT_IRQn);             // enable SCTimer/PWM interrupt
   LPC_SCT->CTRL_L &= ~(1 << 2);         // start timer
}

volatile uint32_t numPwmCycles;
volatile int pwmAborted;
volatile int pwmPhase;

void SCT_IRQHandler(void) {
   uint32_t status = LPC_SCT->EVFLAG;

   if (status & 0x1) {
      ++numPwmCycles; /* Interrupt once per PWM cycle */
   }

   if (status & 0x2) {
      pwmAborted = 1; /* Abort interrupt */
   }

   LPC_SCT->EVFLAG = status; /* Acknowledge interrupts */
}

// int main(void) {
//    uint32_t lastCycles;

//    // Initial Configurations
//    // Enable clocks to relevant peripherals: SWM e SCT
//    LPC_SYSCON->SYSAHBCLKCTRL0 |= (1 << 7) | (1 << 8);
//    // configure the SWM to output CTOUT_0 to PIO0_7 //
//    LPC_SWM->PINASSIGN6 = 0x07ffffff;
//    // Give the module a reset
//    LPC_SYSCON->PRESETCTRL[0] &= ~(1 << 8);  //Assert the SCT reset
//    LPC_SYSCON->PRESETCTRL[0] |= (1 << 8);   //Clear the SCT reset

//    LPC_IOCON->PIO0_0 |= (1 << 5); /* add hysteresis to CTIN_0  */
//    LPC_IOCON->PIO0_7 = 0x80;      /* no pull-up  */
//    LPC_IOCON->PIO0_12 = 0x80;
//    LPC_IOCON->PIO0_16 = 0x80;
//    LPC_IOCON->PIO0_17 = 0x80;

//    // da aggiungere Comandi main iniziali

//    SCT_Init8(); /* Initialize the SCT using code generated   */

//    /* Conflict resolution: Inactive state takes precedence.
// 	 * CTOUT_0, CTOUT_1: Inactive state 0
// 	 * CTOUT_2, CTOUT_3: Inactive state 1  */
//    LPC_SCT->RES = 0x0000005A;

//    NVIC_EnableIRQ(SCT_IRQn);
//    LPC_SCT->CTRL_L &= ~(1 << 2); /* unhalt the SCT by clearing bit 2 of the CTRL register */

//    lastCycles = numPwmCycles; /* Enter an infinite loop */
//    while (1) {
//       if (numPwmCycles != lastCycles) {
//          lastCycles = numPwmCycles;

//          /* Every few PWM cycles change the duty cycles */
//          if ((lastCycles % 5) == 0) {
//             /* Prevent the reload registers from being used before we have updated
// 				 * all PWM channels. */
//             LPC_SCT->CONFIG |= (1u << 7); /* NORELOAD_L (U) */
//             if (pwmPhase == 0) {
//                LPC_SCT->MATCHREL[1].U = 200000;
//                LPC_SCT->MATCHREL[3].U = 700000;
//             } else {
//                LPC_SCT->MATCHREL[1].U = 950000;
//                LPC_SCT->MATCHREL[3].U = LPC_SCT->MATCHREL[0].U;  // duty cycle 0 (test conflict resolution)
//             }
//             /* Update done */
//             LPC_SCT->CONFIG &= ~(1u << 7); /* NORELOAD_L (U) */

//             ++pwmPhase;
//             if (pwmPhase > 1) {
//                pwmPhase = 0;
//             }
//          }
//       }

//       if (pwmAborted) {
//          /* Demo: Poll ABORT input, and restart timer if abort condition has gone. */
//          while (!(LPC_SCT->INPUT & (1u << 0)))
//             LPC_SCT->CTRL |= 2; /*STOP TIMER */
//          LPC_SCT->CTRL |= 4;    /* HALT_L (U) */

//          pwmAborted = 0;
//       }
//    }
//    return 0;
// }

void SCT_RC5_Init(void)  //RC5 receiving
{
   LPC_SYSCON->SYSAHBCLKCTRL0 |= (1 << 8);       // enable the SCT clock
   LPC_SCT->CTRL_L |= (SCC / 1000000 - 1) << 5;  // set prescaler, SCTimer/PWM clock = 1 MHz
   LPC_SCT->REGMODE_L = (1 << 1) | (1 << 2);     // register pair 1 and 2 are capture
   LPC_SCT->MATCH[0].L = 12000;                  // match 0 @ 12000/1MHz = 12 msec (timeout)
   LPC_SCT->MATCHREL[0].L = 12000;
   LPC_SCT->EVENT[0].STATE = 0x00000001;  // event 0 only happens in state 0
   LPC_SCT->EVENT[0].CTRL = (0 << 0) |    // MATCHSEL[3:0] = related to match 0
                            (1 << 12) |   // COMBMODE[13:12] = uses match condition only
                            (1 << 14) |   // STATELD [14] = STATEV is loaded into state
                            (0 << 15);    // STATEV [15] = new state is 0
   LPC_SCT->EVENT[1].STATE = 0x00000001;  // event 1 only happens in state 0
   LPC_SCT->EVENT[1].CTRL = (0 << 6) |    // IOSEL [9:6] = SCT_IN0
                            (2 << 10) |   // IOCOND [11:10] = falling edge
                            (2 << 12) |   // COMBMODE[13:12] = uses IO condition only
                            (1 << 14) |   // STATELD [14] = STATEV is loaded into state
                            (0 << 15);    // STATEV[ 15] = new state is 0
   LPC_SCT->EVENT[2].STATE = 0x00000001;  // event 2 only happens in state 0
   LPC_SCT->EVENT[2].CTRL = (0 << 6) |    // IOSEL [9:6] = SCT_IN0
                            (1 << 10) |   // IOCOND [11:10] = rising edge
                            (2 << 12) |   // COMBMODE[13:12] = uses IO condition only
                            (1 << 14) |   // STATELD [14] = STATEV is loaded into state
                            (0 << 15);    // STATEV [15] = new state is 0
   LPC_SCT->CAPCTRL[1].L = (1 << 1);      // event 1 causes capture 1 to be loaded
   LPC_SCT->CAPCTRL[2].L = (1 << 2);      // event 2 causes capture 2 to be loaded
   LPC_SCT->LIMIT_L = 0x0007;             // events 0, 1 and 2 are used as counter limit
   LPC_SCT->EVEN = 0x00000005;            // events 0 and 2 generate interrupts
   NVIC_EnableIRQ(SCT_IRQn);              // enable SCTimer/PWM interrupt
   LPC_SCT->CTRL_L &= ~(1 << 2);          // unhalt it
}

#define PWM_FREQUENCY 10000       // PWM frequency in Hz
#define SystemCoreClock 30000000  // CLOCK frequency in Hz
#define PWM_RESOLUTION_NS 1000    // Timer resolution in ns
#define PWM_MIN_DUTY_PERCENT 25   // Minimum allowed duty cycle in %
#define PWM_MAX_DUTY_PERCENT 70   // Maximum allowed duty cycle in %
#define SCT_PRESCALER (((SystemCoreClock / 1000u) * PWM_RESOLUTION_NS) / 1000000u - 1u)
#define match_min_width ((10000000u * PWM_MIN_DUTY_PERCENT) / (PWM_FREQUENCY * PWM_RESOLUTION_NS))
#define match_max_width ((10000000u * PWM_MAX_DUTY_PERCENT) / (PWM_FREQUENCY * PWM_RESOLUTION_NS))
#define match_no_input ((10000000u * 300) / (PWM_FREQUENCY * PWM_RESOLUTION_NS))

void SCT_Init(void)  //Decoding PWM
{
   LPC_SCT->CONFIG |= (1 << 0) | (1 << 17);  // unified, auto limit
   LPC_SCT->CTRL |= (SCT_PRESCALER << 5);    // set prescaler
   LPC_SCT->REGMODE_L = 0x00000018;          // 3x MATCH, 2x CAPTURE used
   LPC_SCT->MATCH[0].U = match_max_width;    // match_max_width
   LPC_SCT->MATCHREL[0].U = match_max_width;
   LPC_SCT->MATCH[1].U = match_min_width;  // match_min_width
   LPC_SCT->MATCHREL[1].U = match_min_width;
   LPC_SCT->MATCH[2].U = match_no_input;  // match_no_input
   LPC_SCT->MATCHREL[2].U = match_no_input;
   LPC_SCT->EVENT[0].STATE = 0xFFFFFFFF;            // event 0 happens in all states
   LPC_SCT->EVENT[0].CTRL = (2 << 0) | (1 << 12);   // related to match_no_input only
   LPC_SCT->EVENT[1].STATE = 0xFFFFFFFF;            // event 1 happens in all states
   LPC_SCT->EVENT[1].CTRL = (1 << 10) | (2 << 12);  // IN_0 rising edge condition only
   LPC_SCT->EVENT[2].STATE = (1 << 0);              // event 2 happens in state 0
   LPC_SCT->EVENT[2].CTRL = (1 << 0) |              // related to match_min_width
                            (1 << 12) |             // match condition only
                            (1 << 14) |             // STATEV is loaded into state
                            (1 << 15);              // new state is 1
   LPC_SCT->EVENT[3].STATE = (1 << 1);              // event 3 happens in state 1
   LPC_SCT->EVENT[3].CTRL = (2 << 10) |             // IN_0 falling edge
                            (2 << 12) |             // IO condition only
                            (1 << 14) |             // STATEV is loaded into state
                            (0 << 15);              // new state is 0
   LPC_SCT->EVENT[4].STATE = (1 << 1);              // event 4 happens in state 1
   LPC_SCT->EVENT[4].CTRL = (0 << 0) |              // related to match_max_width
                            (1 << 12) |             // match condition only
                            (1 << 14) |             // STATEV is loaded into state
                            (0 << 15);              // new state is 0
   LPC_SCT->EVENT[5].STATE = (1 << 0);              // event 5 happens in state 0
   LPC_SCT->EVENT[5].CTRL = (2 << 10) | (2 << 12);  // IN_0 falling edge condition only
   LPC_SCT->CAPCTRL[3].U = (1 << 1);                // event 1 is causing capture 3
   LPC_SCT->CAPCTRL[4].U = (1 << 3) | (1 << 5);     // event 3 and 5 cause capture 4
   LPC_SCT->OUT[0].SET = (1 << 1);                  // event 1 set OUT0 (no timeout)
   LPC_SCT->OUT[0].CLR = (1 << 0);                  // event 0 clear OUT0 (timeout)
   LPC_SCT->OUT[1].SET = (1 << 3);                  // event 3 set OUT1 (no width error)
   LPC_SCT->OUT[1].CLR = (1 << 0) | (1 << 5);       // event 0 and 5 clear OUT1 (width error)
   LPC_SCT->OUTPUT |= 3;                            // default set OUT0 and OUT1
   LPC_SCT->LIMIT_L = (1 << 0) | (1 << 1);          // event 0 and 1 limit the timer
   LPC_SCT->EVEN = (1 << 0) | (1 << 5);             // event 0 and 5 generate an irq
   NVIC_EnableIRQ(SCT_IRQn);                        // enable SCTimer/PWM interrupt
   LPC_SCT->CTRL &= ~(1 << 2);                      // start timer
}
