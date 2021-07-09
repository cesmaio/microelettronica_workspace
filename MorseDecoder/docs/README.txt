${ProjName}:
MorseDecoder

TODO:

-x imposta l'unità temporale, con indicatore luminoso (led on board)
- leggi l'input del capacitive touch (lettore codice morse)
    Imposta il CapT come input nella Switch Matrix per il CTimer
- interpreta punto o tratto o pausa in base alla durata del contatto (capT premuto)
- Decodifica il codice morse inserito e stampa a schermo (real time - parola per parola)
- Imposta una sequenza di "DAC mode" -> si aspetta l'inserimento di 4 cifre decimali (4 numeri) 
  che verranno convertiti in un segnale analogico (per un ipotetico output analogico)

- utilizza SCTimer con 2 stati:
  - STATO 0: Modalità di lettura, il capT viene letto e si "scrive" il codice morse
            Il led verde lampeggia per indicare l'unità temporale
  - STATO 1: Modalità di scrittura, il capT viene disabilitato, si scrive a schermo il codice tradotto fin ora
            Il led Rosso lampeggia in pwm ("sto scrivendo")

- BONUS: personalizzazione di unità temporale 
  lascia che l'utente decida l'unità temporale facendo tap sul capT


Intended purpose:
To evaluate the IDE-hw functionality.


Functional description:
* The currently referenced LED is lit.
* A for cycle is used to create a delay.
* The LED is turned off.
* A for cycle is used to create a delay.
* The process repeats.


External connections:
* None


Detailed Program Flow:
* This example runs at 30 MHz.
  See function SystemInit() and chip_setup.h.


* main() routine
   1. Turn on clocks to peripherals
   2. Enter the main while(1) loop
      A. Drive the LED as outlined above

  
This example runs from Flash. 


To run this code: 1. Build
                  2. Load to flash
                  3. Reset
              or:
                  1. Build
                  2. Download and Debug
                  3. Run or Go


Note: It may be necessary to power-cycle the board in order to regain control of the reset button after programming.





