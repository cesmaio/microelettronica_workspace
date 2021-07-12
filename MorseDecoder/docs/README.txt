${ProjName}:
MorseDecoder

TODO:

-x imposta l'unità temporale, con indicatore luminoso (led on board)
- leggi l'input del capacitive touch (lettore codice morse)
    Imposta il CapT come input nella Switch Matrix per il CTimer
-x interpreta punto o tratto o pausa in base alla durata del contatto (capT premuto)
-x Decodifica il codice morse inserito e stampa a schermo (real time - lettera per lettera)
- Imposta una sequenza di "DAC mode" -> si aspetta l'inserimento di 4 cifre decimali (4 numeri) 
  che verranno convertiti in un segnale analogico (per un ipotetico output analogico)

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




