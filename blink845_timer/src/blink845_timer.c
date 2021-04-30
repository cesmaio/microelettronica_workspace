
#include "LPC8xx.h"
#include "LED.h"
#include "DELAY.h"

void clock(time_ms, lightTime_ms) {
    ACT_G
    while(1) {
        G_OFF
        DelayMs(time_ms-(lightTime_ms/2));
        G_ON
        DelayMs(lightTime_ms);
    }
}

void thirdClock(time_ms, lightTime_ms) {
    int thirdTime = (time_ms/3)-(lightTime_ms/2);
    ACT_RGB
    while(1){
        G_OFF
        DelayMs(thirdTime);

        R_ON
        DelayMs(lightTime_ms);
        R_OFF

        DelayMs(thirdTime);

        B_ON
        DelayMs(lightTime_ms);
        B_OFF

        DelayMs(thirdTime);

        G_ON
        DelayMs(lightTime_ms);
    }
}

int main(void) {

 	LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<20;   // Turn on clock to GPIO1 (pag. 101 user manual)
 	// lasciando il registro invariato (apparte il BIT desiderato) con l'operaiozione di |=
    
    thirdClock(1000, 50);
    // clock(1000,10);
}


