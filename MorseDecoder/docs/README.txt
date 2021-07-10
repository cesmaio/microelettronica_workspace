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
To translate a morse code written by user


Functional description (ITA):
* The currently referenced LED is lit.
* A for cycle is used to create a delay.
* The LED is turned off.
* A for cycle is used to create a delay.
* The process repeats.


External connections:
* LCD HD44780
* 




