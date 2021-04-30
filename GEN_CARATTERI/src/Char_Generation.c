#include "LPC8xx.h"
#include "HD44780.h"
#include "Delay.h"
#include "LED.h"

unsigned char Pattern1[8] = {255,255,251,251,255,251,251,251}; // elle specchiata orizzontale
unsigned char Pattern2[8] = {255,255,227,227,227,227,227,227}; // elle doppio ruotata
unsigned char Pattern3[8] = {227,227,227,227,227,227,255,255}; // elle specchiata verticale
unsigned char Pattern4[8] = {248,248,248,248,248,248,255,255}; // elle
unsigned char Pattern5[8] = {248,248,248,248,248,248,248,248}; // verticale sin
unsigned char Pattern6[8] = {224,224,224,224,224,224,254,254}; // trattino basso
unsigned char Pattern7[8] = {254,254,224,224,224,224,224,224}; // trattino alto
unsigned char Pattern8[8] = {255,255,227,227,227,227,255,255}; // parte bassa G
unsigned char Pattern9[8] = {255,255,248,248,248,248,255,255};// segmento per B
unsigned char Pattern10[8] = {254,254,230,230,230,230,254,254};//a destra

//{11111000,11100011,11100011,11100000,11100000,11100011,11100011,11100111} Si parte da 224 e si usa 16 8 4 2 1

void CharGen(unsigned char LineOfDots[8], unsigned char addr) {   // uso più memoria del necessario 64 bit ma + semplice...
	unsigned char i, line, address;

	addr &= 0x0F; // Il singolo carattere è contraddistinto solamente da 3 bit
	address = addr<<3;
	PutCommand(CGRAM_ADDRESS(address));
	for (i=0; i<8; i++) {
		line = LineOfDots[i];
		WriteAscii(line);
	}
	PutCommand(LINE1_HOME);
} // end of main

/*int main (void) {
	unsigned char i, patterns;

	patterns = 8;
//	CompletePattern[patterns][] = {,Pattern2,Pattern3,Pattern4,Pattern5,Pattern6,Pattern7,Pattern8};
	InitLCD_4b_2L();
	CharGen(Pattern1, 0);	CharGen(Pattern2, 1);	CharGen(Pattern3, 2);	CharGen(Pattern4, 3);
	CharGen(Pattern5, 4);	CharGen(Pattern6, 5);	CharGen(Pattern7, 6);	CharGen(Pattern8, 7);
	// PARTE DI SCRITTURA
	PutCommand(LINE1_HOME);
	WriteAscii(0);	WriteAscii(1);	WriteAscii(4); 	WriteAscii(18);	WriteAscii(0);	WriteAscii(6); WriteAscii(0);	WriteAscii(1);
	PutCommand(LINE2_HOME);
	WriteAscii(3);	WriteAscii(2);	WriteAscii(3); 	WriteAscii(5);	WriteAscii(3);	WriteAscii(7); WriteAscii(0);	WriteAscii(1);
}*/

int main (void) {
	unsigned char i, patterns;

	patterns = 8;
//	GENERAZIONE DI CARATTERI
	InitLCD_4b_2L();
	CharGen(Pattern1, 0);	CharGen(Pattern2, 1);	CharGen(Pattern3, 2);	CharGen(Pattern4, 3);
	CharGen(Pattern5, 4);	CharGen(Pattern6, 5);	CharGen(Pattern9, 6);	CharGen(Pattern10, 7);
	// PARTE DI SCRITTURA
	PutCommand(LINE1_HOME);
	WriteAscii(0);	WriteAscii(1);	WriteAscii(4); 	WriteAscii(18);	WriteAscii(6);	WriteAscii(7); WriteAscii(0);	WriteAscii(1);
	PutCommand(LINE2_HOME);
	WriteAscii(0);	WriteAscii(1);	WriteAscii(3); 	WriteAscii(5);	WriteAscii(6);	WriteAscii(7); WriteAscii(0);	WriteAscii(1);
}

/*int main (void) {
	unsigned char i, patterns;

	patterns = 8;
//	GENERAZIONE DI CARATTERI
	InitLCD_4b_2L();
	address = 0x11; // primo carattere a 5x10
	PutCommand(CGRAM_ADDRESS(address));
	for (i=0; i<8; i++) {
		line = LineOfDots[i];
		WriteAscii(line);
	}
	// PARTE DI SCRITTURA
	PutCommand(LINE1_HOME);
	WriteAscii(0);	WriteAscii(1);	WriteAscii(4); 	WriteAscii(18);	WriteAscii(6);	WriteAscii(7); WriteAscii(0);	WriteAscii(1);
	PutCommand(LINE2_HOME);
	WriteAscii(0);	WriteAscii(1);	WriteAscii(3); 	WriteAscii(5);	WriteAscii(6);	WriteAscii(7); WriteAscii(0);	WriteAscii(1);
}*/


