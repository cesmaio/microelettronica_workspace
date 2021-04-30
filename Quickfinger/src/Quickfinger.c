/*
===============================================================================
 Name        : Quickfinger.c
 Author      : Cesare Maio
 Version     : 1.0
 Copyright   : Cesare Maio
 Description : a reaction mini-game
===============================================================================
*/
#include "LPC8xx.h"
#include "LED.h"
#include "Delay.h"
#include "HD44780.h"
#include <stdlib.h>

// # definizioni
#define B1_pressed ~(LPC_GPIO_PORT->PIN0 & (1 << 9)) // button 1 pin 18
#define B2_pressed ~(LPC_GPIO_PORT->PIN0 & (1 << 8)) // button 2 pin 19
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

// ## Caratteri personalizzati
unsigned char gun_calcioSx[8] = {0x0C, 0x07, 0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x0E};
unsigned char gun_grillettoSx[8] = {0x00, 0x1F, 0x1F, 0x1F, 0x12, 0x0C, 0x00, 0x00};
unsigned char gun_cannaSx[8] = {0x02, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char sparoDx[8] = {0x0E, 0x1F, 0x1F, 0x0C, 0x01, 0x08, 0x02, 0x00};

unsigned char gun_calcioDx[8] = {0x06, 0x1C, 0x18, 0x1C, 0x1E, 0x1F, 0x1F, 0x0E};
unsigned char gun_grillettoDx[8] = {0x00, 0x1F, 0x1F, 0x1F, 0x09, 0x06, 0x00, 0x00};
unsigned char gun_cannaDx[8] = {0x08, 0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char sparoSx[8] = {0x0E, 0x1F, 0x1F, 0x06, 0x10, 0x02, 0x08, 0x00};

// # Prototipi
unsigned int setupLCD(unsigned int bit);                                          // init LCD 2 lines 4 bit/8 bit
void CharGen(unsigned char LineOfDots[8], unsigned char addr);                    // genera carattere personalizzato e salva in memoria di LCD
void showRules(void);                                                             // mostra le regole
unsigned int writeL_wArrows(unsigned int L, unsigned char line[LCD_LINE_LENGTH]); // scrivi riga su LCD con freccie ai lati (per indicare i bottoni)
void select(void);                                                                // lampeggia il testo selezionato per 3 volte

int main(void)
{
    if (!setupLCD(8))
    {
        R_ON return 1;
    } // accendi LED R on board se c'è un errore
    // writeL(1, "- Quick Finger -");
    // writeL_wArrows(2, "REGOLE   GIOCA");

    CharGen(gun_calcioSx, _gun_calcioSx);
    CharGen(gun_grillettoSx, _gun_grillettoSx);
    CharGen(gun_cannaSx, _gun_cannaSx);
    CharGen(sparoSx, _sparoSx);
    CharGen(gun_calcioDx, _gun_calcioDx);
    CharGen(gun_grillettoDx, _gun_grillettoDx);
    CharGen(gun_cannaDx, _gun_cannaDx);
    CharGen(sparoDx, _sparoDx);

    writeC(_gun_calcioSx);
    writeC(_gun_grillettoSx);
    writeC(_gun_cannaSx);
    writeC(_sparoSx);
    writeC(32);
    writeC(32);
    writeC(_sparoDx);
    writeC(_gun_cannaDx);
    writeC(_gun_grillettoDx);
    writeC(_gun_calcioDx);

    DelayMs(1000);

    // showRules();

} // end main

unsigned int setupLCD(unsigned int bit)
{
    LPC_SYSCON->SYSAHBCLKCTRL0 |= 1 << 20; // turn on GPIO1
    ACT_RGB;

    if (bit == 4)
    {
        InitLCD_4b_2L();
    }
    else if (bit == 8)
    {
        InitLCD_8b_2L();
    }
    else
    {
        return 0; // bit è sbagliato
    }
    PutCommand(DISP_ON_CUR_OFF_BLINK_OFF); // no cursor, no blink
    return 1;
}

void CharGen(unsigned char LineOfDots[8], unsigned char addr)
{ // uso più memoria del necessario 64 bit ma + semplice...
    unsigned char i, line, address;

    addr &= 0x0F; // Il singolo carattere è contraddistinto solamente da 3 bit
    address = addr << 3;
    PutCommand(CGRAM_ADDRESS(address));
    for (i = 0; i < 8; i++)
    {
        line = LineOfDots[i];
        writeC(line);
    }
    PutCommand(LINE1_HOME);
}

void showRules(void)
{
    PutCommand(DISPLAY_CLEAR);

    writeL(1, "---- REGOLE ----");
    writeL(2, "Tocca un bottone");

    int count = 0;
    while (++count < 100)
    {
        if (count % 2 == 0)
        {
            writeL(2, "Tocca un bottone");
            DelayMs(500);
            continue;
        }
        clearL(2);
        DelayMs(500);
    }

    writeL(1, "Quando vedi '!' ");
    writeL(2, " sullo schermo  ");
    writeL(1, "Tocca il bottone");
    writeL(2, "<-per sparare ->");
}

unsigned int writeL_wArrows(unsigned int L, unsigned char line[LCD_LINE_LENGTH])
{
    switchL(L);
    writeC(frecciaSx); // freccia sinistra a inizio riga

    for (unsigned char i = 0; i < (LCD_LINE_VISIBLE - 2); i++) // riserva l'ultimo spazio visible per la freccia
    {
        if (line[i])
            writeC(line[i]);
        else
            writeC(32); // fill line with spaces
    }

    writeC(frecciaDx); // freccia destra a fine riga
}
