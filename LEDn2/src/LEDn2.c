
#include "LPC8xx.h"
#include "LED.h"

/*
 * Pulsante su PIO0_00 -> 39
 * LED esterno su PIO0_01  -> 38
 */
// int main(void) {

//     LPC_SYSCON->SYSAHBCLKCTRL0 |= (1<<20 | 1<<6); // Turn on clock to GPIO1 & GPIO0

//     // premendo CTRL + SPACE sull'istruzione si attivano tutte le stringhe di comando riconosciute dal sistema
//     LPC_GPIO_PORT->CLR0 |= 0x2; // scrivendo un valore alto sul registro di CLEAR (CLR) abbassiamo il valore del PIN
//     LPC_GPIO_PORT->DIRSET0 |= 0x2; // attiviamo la porta come uscita

//     ACT_R
// 	ACT_B

//     while(1) {
//        R_ON

// 		// negazione logica: (~ -> bit a bit) (! -> bit singolo)
//        if (!(LPC_GPIO_PORT->PIN0 & 0x1)) { 
//         // mascheriamo il valore del PIN0 (39) con il valore alto (1) = pulsante non premuto
//     	   B_ON
//     	   LPC_GPIO_PORT->SET0 |= 0x2; // LED esterno ON
//        } else {
//     	   B_OFF
//     	   LPC_GPIO_PORT->CLR0 |= 0x2; // LED esterno OFF
//        }
//     } // end of while(1)

// } // end of main


/*
 * Es:
 * Svolgere lo stesso compito di accensione del LED esterno senza utilizzare la GPIO0
 * Sfruttare la sola GPIO1 (che controlla i led interni) per controllare anche i LED esterni
 */


/*
 * Pulsante su PIO1_1 -> 30
 * LED esterno su PIO1_0 (GREEN) -> 29
 */
int main(void) {

	uint32_t btn_count = 0, pressed = 0, rimbalzi = 0;

    LPC_SYSCON->SYSAHBCLKCTRL0 |= 1<<20; // Turn on clock to GPIO1 (non utilizziamo GPIO0)

    ACT_R
	ACT_G

    while(1) {

       if (!(LPC_GPIO_PORT->PIN1 & 0x2)) { // pulsante premuto
    	   G_OFF // Spengo il led G e Accendo il LED esterno
		   R_ON

           if (++btn_count == 1) pressed = 1;
       } else {
    	   R_OFF
		   G_ON // accendo green -> spengo LED esterno (non vedo GREEN perchè RED lo sovrasta)
           
           if (pressed) rimbalzi++;
       }
    } // end of while(1)

} // end of main
