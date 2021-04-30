
#include "LPC8xx.h"
#include "LED.h"

void Delay_sw_ms(uint32_t delayinms)
{
	uint32_t i;
	for(i=0;i<(delayinms*880);i++);
}

int main(void) {
    uint32_t timeon;

    LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<20;   // Turn on clock to GPIO1 (pag. 101 user manual)
     	// lasciando il registro invariato (apparte il BIT desiderato) con l'operaiozione di |=

    ACT_RGB
    timeon = 1500;
    while(1) {
        B_ON        Delay_sw_ms(timeon);
        G_ON        Delay_sw_ms(timeon);
        R_ON        Delay_sw_ms(timeon);
// (BGR) se cambio l'ordine di accensione dei led, non funziona:
// Questo perchè la tensione richiesta per l'accesione di ogni led è maggiore (il led R copre gli altri due)
        RGB_OFF
    } // end of while(1)

} // end of main
