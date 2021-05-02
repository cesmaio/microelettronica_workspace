/*
===============================================================================
 Name        : Quickfinger.c
 Author      : Cesare Maio
 Version     : 1.0
 Copyright   : Cesare Maio
 Description : a reaction mini-game
===============================================================================
*/
#include <stdlib.h>

#include "Delay.h"
#include "HD44780.h"
#include "LED.h"
#include "LPC8xx.h"

// # definizioni
#define B1_pressed ~(LPC_GPIO_PORT->PIN0 & (1 << 9))  // button 1 pin 18
#define B2_pressed ~(LPC_GPIO_PORT->PIN0 & (1 << 8))  // button 2 pin 19
#define frecciaDx 0x7E
#define frecciaSx 0x7F
#define dot 0xA5
// LCD symbols
#define _gun_calcioSx 0
#define _gun_grillettoSx 1
#define _gun_cannaSx 2
#define _sparoSx 3
#define _gun_calcioDx 4
#define _gun_grillettoDx 5
#define _gun_cannaDx 6
#define _sparoDx 7

// # Caratteri personalizzati
unsigned char gun_calcioSx[8] = {0x0C, 0x07, 0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x0E};
unsigned char gun_grillettoSx[8] = {0x00, 0x1F, 0x1F, 0x1F, 0x12, 0x0C, 0x00, 0x00};
unsigned char gun_cannaSx[8] = {0x02, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char sparoDx[8] = {0x0E, 0x1F, 0x1F, 0x0C, 0x01, 0x08, 0x02, 0x00};

unsigned char gun_calcioDx[8] = {0x06, 0x1C, 0x18, 0x1C, 0x1E, 0x1F, 0x1F, 0x0E};
unsigned char gun_grillettoDx[8] = {0x00, 0x1F, 0x1F, 0x1F, 0x09, 0x06, 0x00, 0x00};
unsigned char gun_cannaDx[8] = {0x08, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char sparoSx[8] = {0x0E, 0x1F, 0x1F, 0x06, 0x10, 0x02, 0x08, 0x00};

unsigned char findChars[3] = {'<', '>', ':'};
unsigned char replaceChars[3] = {frecciaSx, frecciaDx, dot};

// # Prototipi
unsigned int setupLCD(unsigned int bit);                        // init LCD 2 lines 4 bit/8 bit
void intro();                                                   // mostra la schermata di intro del gioco
void CharGen(unsigned char LineOfDots[8], unsigned char addr);  // genera carattere personalizzato e salva in memoria di LCD
void showRules(void);                                           // mostra le regole
unsigned int selectMode(void);                                  // seleziona modalità di gioco, 1 -> 1vs1 2 -> vsCOM
unsigned int ready(unsigned int mode);                          // tutto pronto per giocare? 1 = si, 0 = no
void play(unsigned int mode);                                   // avvia il gioco con la modalità selezionata
unsigned int shootAt(unsigned int player);                      // animazione su LCD quando uno dei due giocatori spara
unsigned int playagain(void);                                   // mostra il messaggio "vuoi giocare di nuovo?"

// # MAIN
int main(void) {
    if (!setupLCD(8)) {
        R_ON return 1;
    }  // accendi LED R on board se c'è un errore

    do {
        int seed = -1;  // seed per generare numeri random
        int mode = 0;   // modalità di gioco

        // INTRO
        writeL(1, "---- REGOLE ----* Quick Finger *---- GIOCA -----");
        writeL_replace(2, "Tocca un bottone<REGOLE   GIOCA><1vs1     vsCOM>", findChars, replaceChars);
        DisplayRight(16);  // schermata principale

        // seeding (credits to Lorenzo Sabino)
        int count = 0;
        while (seed == -1) {  // aspetta che venga premuto un pulsante
            DelayMs(1);
            count++;
            if (B1_pressed) {
                slideDisplay("l", 100);
                showRules();
            } else if (B2_pressed) {
                seed = count;  // salva seed
                slideDisplay("r", 100);
                mode = selectMode();  // 1 = 1vs1, 2 = vsCOM
            }
        }
        srand(seed);  // seed rand

        if (ready(mode))
            play(mode);  // loop principale
    } while (playagain());

    return 0;
}  // end main

unsigned int setupLCD(unsigned int bit) {
    LPC_SYSCON->SYSAHBCLKCTRL0 |= 1 << 20;  // turn on GPIO1
    ACT_RGB;

    if (bit == 4) {
        InitLCD_4b_2L();
    } else if (bit == 8) {
        InitLCD_8b_2L();
    } else {
        return 0;  // bit è sbagliato
    }
    PutCommand(DISP_ON_CUR_OFF_BLINK_OFF);  // no cursor, no blink
    return 1;
}

void CharGen(unsigned char LineOfDots[8], unsigned char addr) {  // uso più memoria del necessario 64 bit ma + semplice...
    unsigned char i, line, address;

    addr &= 0x0F;  // Il singolo carattere è contraddistinto solamente da 3 bit
    address = addr << 3;
    PutCommand(CGRAM_ADDRESS(address));
    for (i = 0; i < 8; i++) {
        line = LineOfDots[i];
        writeC(line);
    }
    PutCommand(LINE1_HOME);
}

void showRules(void) {
    // aspetta che venga premuto un pulsante
    int count = 0;
    while (B1_pressed || B2_pressed) {  // blink line
        if (++count % 2 == 0)
            writeL(2, "Tocca un bottone");
        else
            clearL(2);

        DelayMs(500);
    }

    writeL(1, "Quando vedi '!' ");
    writeL(2, "sullo schermo...");
    while (B1_pressed || B2_pressed)
        ;
    writeL(1, "Tocca il bottone");
    writeL_replace(2, "< per sparare  >", findChars, replaceChars);
    while (B1_pressed || B2_pressed)
        ;
    writeL(1, "chi spara primo ");
    writeL(2, "ottiene +1 punto");
    while (B1_pressed || B2_pressed)
        ;
    writeL(1, "il giocatore che");
    writeL(2, "arriva a 3 VINCE");
    while (B1_pressed || B2_pressed)
        ;
}

unsigned int selectMode(void) {
    unsigned int mode = 0;

    // aspetta che venga premuto un pulsante
    int count = 0;
    while (B1_pressed || B2_pressed) {  // blink line
        if (++count % 2 == 0)
            writeL_replace(2, "<1vs1     vsCOM>", findChars, replaceChars);
        else
            clearL(2);

        DelayMs(500);

        if (B1_pressed) {
            // seleziona 1v1, mostra il testo a Sx lampeggiante per indicare la selezione
            writeBlinkL(2, 1, "1vs1", 3, 100);
            mode = 1;
            break;
        } else if (B2_pressed) {
            // seleziona COM, mostra testo lampeggiante a Dx
            writeBlinkL(2, 13, "vsCOM", 3, 100);
            mode = 2;
            break;
        }
    }
    DelayMs(300);

    return mode;
}

unsigned int ready(unsigned int mode) {
    PutCommand(DISPLAY_CLEAR);

    // 1vs1
    if (mode == 1) {
        writeL(1, "    PRONTI ?    ");
        unsigned char line2[16] = "<G1          G2>";

        // aspetta che vengano premuti entrambi i pulsanti
        unsigned int G1_ready = 0, G2_ready = 0;
        int count = 0;
        while (!(G1_ready && G2_ready)) {  // blink line
            if (++count % 2 == 0)
                writeL_replace(2, line2, findChars, replaceChars);
            else
                clearL(2);

            DelayMs(500);

            if (B1_pressed) {
                writeToXY(4, 1, "OK!");
            }
            if (B2_pressed) {
                writeToXY(8, 1, "OK!");
            }
        }
        return 1;
    }
    // vsCOM
    else if (mode == 2) {
        writeL(1, "Non ancora");
        writeL(2, "disponibile");
        return 0;
    }

    return 0;
}

unsigned int play(unsigned int mode) {
    unsigned int round = 0;            // numero di round
    unsigned int playing = 1;          // controllo per il loop di gioco
    int points_G1 = 0, points_G2 = 0;  // punteggi dei giocatori
    int cheat_G1 = 0, cheat_G2 = 0;    // tieni il conto di quante volte un giocatore bara (spara prima di '!')
    unsigned int cheating = 0;         // controlla chi sta barando 1 o 2
    unsigned int winning = 0;          // controlla chi ha vinto un punto 1 0 2
    unsigned int finished = 0;         // controlla se il gioco è finito, ha vinto 1 o 2

    // ## genera i caratteri personalizzati per le pistole
    CharGen(gun_calcioSx, _gun_calcioSx);
    CharGen(gun_grillettoSx, _gun_grillettoSx);
    CharGen(gun_cannaSx, _gun_cannaSx);
    CharGen(sparoSx, _sparoSx);
    CharGen(gun_calcioDx, _gun_calcioDx);
    CharGen(gun_grillettoDx, _gun_grillettoDx);
    CharGen(gun_cannaDx, _gun_cannaDx);
    CharGen(sparoDx, _sparoDx);

    // loop principale
    do {
        PutCommand(DISPLAY_CLEAR);
        // scrivi punteggi
        writeL(1, "G1:         G2: ");
        writeToXY(3, 0, "");
        Write_ndigitsval(points_G1, 1);
        writeToXY(15, 0, "");
        Write_ndigitsval(points_G2, 1);

        DelayMs(200);

        // print guns
        PutCommand(LINE2_HOME);
        writeC(_gun_calcioSx);
        writeC(_gun_grillettoSx);
        writeC(_gun_cannaSx);
        writeToXY(13, 1, _gun_cannaDx);
        writeC(_gun_grillettoDx);
        writeC(_gun_calcioDx);

        DelayMs(500);

        // scrivi round
        writeToXY(5, 0, "ROUND");
        Write_ndigitsval(++round; 1);

        DelayMS(1000);

        writeToXY(4, 0, "        ");  // cancella scritta round

        DelayMs(100);

        // genera numero random di punti (da 2 a 7) prima del '!'
        uint8_t wait_dots = rand() % 6 + 2;
        unsigned char _dot[1] = {dot};

        uint8_t i = 0;  // contatore dei punti
        for (; i < wait_dots; i++) {
            writeToXY(4 + i, 0, _dot);
            DelayMs(1000);

            // se un giocatore preme  il pulsante prima di '!' sta barando
            if (B1_pressed) {
                cheat_G1++;
                shootAt(2);
                cheating = 1;
                break;
            }
            if (B2_pressed) {
                cheat_G2++;
                shootAt(1);
                cheating = 2;
                break;
            }
        }

        if (!cheating) {
            writeToXY(4 + i, 0, "!");

            do {  // controlla chi spara per primo
                if (B1_pressed) {
                    points_G1++;
                    shootAt(2);
                    winning = 1;
                } else if (B2_pressed) {
                    points_G2++;
                    shootAt(1);
                    winning = 2;
                }
            } while (!winning);

            DelayMS(1000);

            // assegna il punto al vincitore, fai sparire il fumo dello sparo
            writeToXY(3, 1, "    G");
            Write_ndigitsval(winning, 1);
            WriteAfter(" +1    ");

            DelayMS(3000);

            winning = 0;
        } else {  // qualcuno ha barato
            DelayMS(1000);
            int cheat_count = 0;
            if (cheating == 1)
                cheat_count = cheat_G1;
            else if (cheating == 2)
                cheat_count = cheat_G2;
            else
                return 1;  // error

            // segnala chi ha barato

            switch (cheat_count) {
                case 1:
                    writeL(1, "G  tieni a bada ");
                    writeL(2, " il grilletto!  ");
                    writeToXY(1, 0, "");
                    break;
                case 2:
                    writeL(1, " G  riprovaci e ");
                    writeL(2, "   sei fuori!   ");
                    writeToXY(2, 0, "");
                    break;
                case 3:  // dopo la terza volta si perde automaticamente
                    if (cheating == 1)
                        finished = 2;  // vince G2
                    else
                        finished = 1;  // vince G1

                    writeL(1, "G  ora e' troppo");
                    writeL(2, "ti sei suicidato");
                    writeToXY(1, 0, "");
                    break;
                default:
                    break;
            }
            Write_ndigitsval(cheating, 1);

            DelayMS(5000);

            cheating = 0;
            round--;  // ripeti il round
        }

        // controlla se c'è un vincitore, il gioco è finito
        if (points_G1 == 3) {
            finished = 1;
        } else if (points_G2 == 3) {
            finished = 2;
        } else
            continue;

        // abbiamo un vincitore
        if (finished) {
            // scrivi punteggi
            writeL(1, "G1:         G2: ");
            writeToXY(3, 0, "");
            Write_ndigitsval(points_G1, 1);
            writeToXY(15, 0, "");
            Write_ndigitsval(points_G2, 1);

            clearL(2);
            writeL(2, "     G");
            Write_ndigitsval(finished, 1);
            writeBlinkL(2, 8, "VINCE!", 5, 500);

            playing = 0;
        }

    } while (playing);

    return 0;
}

unsigned int shootAt(unsigned int player) {
    // spara al giocatore 1 0 2
    // animazione
    if (player == 1) {
        // G2 spara
        writeToXY(12, 1, "");
        writeC(_sparoDx);
        DelayMS(500);
        unsigned char gunSx[3] = {_gun_calcioSx, _gun_grillettoSx, _gun_calcioSx};
        writeBlinkL(2, 0, gunSx, 3, 100);
    } else if (player == 2) {
        // G1 spara
        writeToXY(3, 1, "");
        writeC(_sparoSx);
        DelayMS(500);
        unsigned char gunDx[3] = {_gun_cannaDx, _gun_grillettoDx, _gun_calcioDx};
        writeBlinkL(2, 0, gunSx, 3, 100);
    } else
        return 1;  // error

    return 0;
}

unsigned int playagain(void) {
    writeL(1, "Tocca un bottone");
    writeL(2, "per ricominciare");
    while (B1_pressed || B2_pressed)
        ;
}