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

// definizioni
#define B1_pressed !(LPC_GPIO_PORT->PIN0 & (1 << 9)) // button 1 pin 18
#define B2_pressed !(LPC_GPIO_PORT->PIN0 & (1 << 8)) // button 2 pin 19
#define frecciaDx 0x7E
#define frecciaSx 0x7F
#define dot 0xA5

// # Prototipi
unsigned int setupLCD(unsigned int bit);                                          // init LCD 2 lines 4 bit/8 bit
void showRules(void);                                                             // mostra le regole
unsigned int writeL_wArrows(unsigned int L, unsigned char line[LCD_LINE_LENGTH]); // scrivi riga su LCD con freccie ai lati (per indicare i bottoni)
void select(void);                                                                // lampeggia il testo selezionato per 3 volte

int main(void)
{

    if (!setupLCD(8))
    {
        R_ON return 1;
    } // accendi LED R on board se c'è un errore
    writeL(1, "- Quick Finger -");
    writeL_wArrows(2, "REGOLE   GIOCAAAAAAAA");

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
    WriteAscii(frecciaSx); // freccia sinistra a inizio riga

    for (unsigned char i = 0; i < (LCD_LINE_VISIBLE - 2); i++) // riserva l'ultimo spazio visible per la freccia
    {
        if (line[i])
            WriteAscii(line[i]);
        else
            WriteAscii(32); // fill line with spaces
    }

    WriteAscii(frecciaDx); // freccia destra a fine riga
}
