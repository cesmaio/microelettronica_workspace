/* ###################################################################
**	Filename: HD44780.c
**  Project: 
**	Processor: LPC2103
**	Compiler: gcc version 4.1.1
**	Date: 16/11/2009
**	Abstract:
**		HD44780.c source file (user routines)
** 	Modified: 
** 			InitLCD() function: initializing by instruction procedure
** ###################################################################*/

#include "HD44780.h"

unsigned char BitNum;
/*****************************************************************
*****  LCD initialization routine  (4 bit/8bit interface defined)  ****
*****************************************************************/
void  InitLCD_8b_1L(void)
{
	LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<6;   // Turn on clock to GPIO0
	INIT_BUS		 //P0.[23:16,1:0] definition: FULL_BUS_CLEAR; FULL_BUS_OUT
	DelayMs(10);	 // Delay a minimum of time serves when LCD is just plugged

/*/ This part is for working with DISPLAY_FLAG and it is on 8 bit interface
	LPC_GPIO_PORT->SET[0] = DB5_PIN; // DB7=0, DB6=0, DB5=1, DB4=1,
	LPC_GPIO_PORT->SET[0] = DB4_PIN; // NU=0, EN=0, RW=0, RS=0
	delayUs(1);		 // Wait a minimum of 60ns
		//repeat (function set) 3 times 
	LPC_GPIO_PORT->SET[0] = EN;		 // Raise EN start write operation
	delayUs(1);		 // Wait a minimum of 195ns
	LPC_GPIO_PORT->CLR[0] = EN;		 // Clear EN finish write operation
	delayMs(5);		 // Wait a minimum of 4.1 ms

	LPC_GPIO_PORT->SET[0] = EN;		 // Raise EN start write operation
	delayUs(1);		 // Wait a minimum of 195ns
	LPC_GPIO_PORT->CLR[0] = EN;		 // Clear EN finish write operation
	delayUs(200);	 // Wait a minimum of 100 us

	LPC_GPIO_PORT->SET[0] = EN;		 // Raise EN start write operation
	delayUs(1);		 // Wait a minimum of 195ns
	LPC_GPIO_PORT->CLR[0] = EN;		 // Clear EN finish write operation
	delayUs(100);	 // Wait a minimum of 100 us
// This part is on 8 bit interface*/

	LPC_GPIO_PORT->SET[0] = EN;		 // Raise EN start write operation
	LPC_GPIO_PORT->SET[0] = DB5_PIN; // DB7=0, DB6=0, DB5=1, DB4=1, NU=0, EN=0, RW=0, RS=0
	LPC_GPIO_PORT->SET[0] = DB4_PIN; // SETS 8-bit operation
//	LPC_GPIO_PORT->SET[0] = 1<<(DB4_PIN_SHIFT-1); // Select 2 line-Display
	DelayUs(1);		 // Wait a minimum of 195ns
	LPC_GPIO_PORT->CLR[0] = EN;		 // Clear EN finish write operation
	DelayUs(10);		 // Wait a minimum between the two cycles
	LPC_GPIO_PORT->SET[0] = EN;		 // Raise EN start write operation
	DelayUs(1);
	LPC_GPIO_PORT->CLR[0] = EN;		 // Raise EN start write operation
	BitNum = 1;
	PutCommand(ENTRY_MODE_INC_NO_SHIFT);
	PutCommand(DISP_ON_CUR_ON_BLINK_ON);

	PutCommand(DISPLAY_CLEAR);
	DelayMs(2);
}

void  InitLCD_8b_2L(void) // Serve per chi alimenta a 5V o usa l'integrato
{
	LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<6;   // Turn on clock to GPIO0
	INIT_BUS		 //P0.[23:16,1:0] definition: FULL_BUS_CLEAR; FULL_BUS_OUT
	DelayMs(10);	 // Delay a minimum of time serves when LCD is just plugged
	LPC_GPIO_PORT->SET[0] = EN;		 // Raise EN start write operation
	LPC_GPIO_PORT->SET[0] = DB5_PIN; // DB7=0, DB6=0, DB5=1, DB4=1, NU=0, EN=0, RW=0, RS=0
	LPC_GPIO_PORT->SET[0] = DB4_PIN; // SETS 8-bit operation
	LPC_GPIO_PORT->SET[0] = 1<<(DB4_PIN_SHIFT-1); // set the number of lines
	DelayUs(1);		 // Wait a minimum of 195ns
	LPC_GPIO_PORT->CLR[0] = EN;		 // Clear EN finish write operation
	DelayUs(10);		 // Wait a minimum between the two cycles
	LPC_GPIO_PORT->SET[0] = EN;		 // Raise EN start write operation
	DelayUs(1);
	LPC_GPIO_PORT->CLR[0] = EN;		 // Raise EN start write operation
	BitNum = 1;
	PutCommand(ENTRY_MODE_INC_NO_SHIFT);
	PutCommand(DISP_ON_CUR_ON_BLINK_ON);
	PutCommand(DISPLAY_CLEAR);
	DelayMs(2);
}

void  InitLCD_4b_1L(void) // Alternativa in uso a 3.3 V con più PIN liberi
{
	LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<6;   // Turn on clock to GPIO0
	INIT_BUS;		 //P0.[23:16,1:0] definition: FULL_BUS_CLEAR; FULL_BUS_OUT
	DelayMs(10);	 // Delay a minimum of time serves when LCD is just plugged
	LPC_GPIO_PORT->SET[0] = EN;		 // Raise EN start write operation
	LPC_GPIO_PORT->SET[0] = DB5_PIN; // DB7=0, DB6=0, DB5=1, DB4=0, NU=0, EN=0, RW=0, RS=0
	DelayUs(1);		 // Wait a minimum of 195ns
	LPC_GPIO_PORT->CLR[0] = EN;		 // Clear EN finish write operation
	DelayUs(100);	 // Delay a minimum of 100 us
	DEINIT_BUS;		 //P0.[19:16] liberiamo ora i primi 4 pin
	BitNum = 0;
	PutCommand(ENTRY_MODE_INC_NO_SHIFT);
	PutCommand(DISP_ON_CUR_ON_BLINK_ON);
	PutCommand(DISPLAY_CLEAR);
	DelayMs(2);
}


void  InitLCD_4b_2L(void)
{
	LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<6;   // Turn on clock to GPIO0
	INIT_BUS;		 //P0.[23:20,1:0] definition: FULL_BUS_CLEAR; FULL_BUS_OUT
	DelayMs(10);	 // Delay a minimum of time serves when LCD is just plugged
	LPC_GPIO_PORT->SET[0] = EN;		 // Raise EN start write operation
	LPC_GPIO_PORT->SET[0] = DB5_PIN; // DB7=0, DB6=0, DB5=1, DB4=0, 4-bit
	DelayUs(1);		 // Wait a minimum of 195ns
	LPC_GPIO_PORT->CLR[0] = EN;		 // Clear EN finish write operation

	DEINIT_BUS;		 //P0.[19:16] liberiamo ora i primi 4 pin
	DelayUs(100);	 // Delay a minimum of 100 us
	BitNum = 0;
	PutCommand(DISP_ON_CUR_ON_BLINK_ON);
	PutCommand(ENTRY_MODE_INC_NO_SHIFT);
	PutCommand(FOUR_BIT_TWO_LINE_5x8); // Select 2 line-Display
	DelayUs(100);	 // Delay a minimum

	PutCommand(DISPLAY_CLEAR);
	DelayMs(2);
}

/**************************************
*****    Send a command to LCD     ****
***************************************/
void PutCommand(unsigned char Command)
{
	DelayMs(1);							// Wait until LCD is free
	WriteByte(ISTR, Command);		// Write character to IR
}

/**************************************************
*****    Write a string on LCD visible part    ****
***************************************************/
void WriteInitial(unsigned char LineOfCharacters[LCD_LINE_VISIBLE])
{
	unsigned char i=0;

	PutCommand(LINE1_HOME);
	DelayUs(100); // Ci riporta all'inizio della memoria del Display
	while(LineOfCharacters[i] && i<LCD_LINE_VISIBLE)
	{
		WriteAscii(LineOfCharacters[i]);
		i++;
	}
}

/**************************************************
*****    Write a string on LCD total line      ****
***************************************************/
void WriteAll(unsigned char lineOfCharacters[LCD_LINE_LENGHT])
{
	unsigned char i=0;

	PutCommand(LINE1_HOME); // Ci riporta a scrivere all'inizio della memoria del Display
	DelayUs(100);
	while(lineOfCharacters[i] && i<LCD_LINE_LENGHT)
	{
		WriteAscii(lineOfCharacters[i]);
		i++;
	}
}

/**************************************
*****   Write a character on LCD   ****
***************************************/
void WriteAscii(unsigned char symbol)
{
	DelayMs(1);							// Wait until LCD is free
	WriteByte(DATA, symbol);	// Write character to DR
}


/**************************************
*****     Write a byte on LCD      ****
***************************************/
void WriteByte(unsigned char rs, unsigned char data_to_LCD)
{
	unsigned int dataVal;
	
	if(rs)											
		LPC_GPIO_PORT->SET[0] = RS;								// Set RS (DR operation)
	else
		LPC_GPIO_PORT->CLR[0] = RS;								// Clear RS (IR operation)

	DelayUs(1);									// Wait a minimum of 60ns
	if (BitNum)
		{LPC_GPIO_PORT->SET[0] = EN;									// Set EN in order to start first write operation
		dataVal = data_to_LCD << (DB4_PIN_SHIFT-4);		// shift left on dataVal to align DB7:DB4 on DATA_BUS
		LPC_GPIO_PORT->PIN[0] = (LPC_GPIO_PORT->PIN[0] & ~DATA_BUS)|dataVal;		// data bus reflects upper nibble
		DelayUs(1);									// Wait a minimum of 195ns
		LPC_GPIO_PORT->CLR[0] = EN;									// Clear EN finish first write operation
		DelayUs(1);} // Wait a minimum of 530ns between writes*/
	else	{
		LPC_GPIO_PORT->SET[0] = EN;									// Set EN in order to start first write operation
		dataVal = data_to_LCD & 0xF0;				// Copy upper nibble into dataVal
		dataVal = dataVal << (DB4_PIN_SHIFT-4);		// shift left on dataVal to align DB7:DB4 on DATA_BUS
		LPC_GPIO_PORT->PIN[0] = (LPC_GPIO_PORT->PIN[0] & ~DATA_BUS)|dataVal;		// data bus reflects upper nibble
		DelayUs(1);									// Wait a minimum of 195ns
		LPC_GPIO_PORT->CLR[0] = EN;									// Clear EN finish first write operation
		DelayUs(1);									// Wait a minimum of 530ns between writes
		LPC_GPIO_PORT->SET[0] = EN;									// Raise EN start second write operation
		dataVal = data_to_LCD & 0x0F;				// Copy lower nibble into dataVal
		dataVal = dataVal << DB4_PIN_SHIFT;			// shift left on dataVal to align DB4
		LPC_GPIO_PORT->PIN[0] = (LPC_GPIO_PORT->PIN[0] & ~DATA_BUS)|dataVal;		// data bus reflects lower nibble
		DelayUs(1);									// Wait a minimum of 195ns
		LPC_GPIO_PORT->CLR[0] = EN;									// Clear EN finish first write operation
		DelayUs(1);
	}
}


/*************************************************************
*****    Write in line after current position on LCD     ****
**************************************************************/
void WriteAfter(unsigned char LineOfCharacters[LCD_LINE_LENGHT])
{
	unsigned char i=0;

	while(LineOfCharacters[i] && i<LCD_LINE_LENGHT)
	{
		WriteAscii(LineOfCharacters[i]);
		i++;
	}
}


/**************************************
*****     Write 2 digit on LCD     ****
***************************************/
void Write_2digitsval(unsigned int dummyVal)
{
	WriteAscii(NUM_TO_CODE(dummyVal/10));
	dummyVal %= 10;
	WriteAscii(NUM_TO_CODE(dummyVal));
}

/**************************************
*****     Write n digit on LCD     ****
***************************************/
void Write_ndigitsval_space(unsigned int dummyVal, unsigned char ndigits)
{
	int ten_base=1, digit, leading_zeroes_flag = 1;

	while(--ndigits)
		ten_base*=10;

	while(ten_base)
	{
		digit = dummyVal/ten_base;
		if(digit)
			leading_zeroes_flag = 0;
		WriteAscii(leading_zeroes_flag&&(ten_base>1)? ' ': NUM_TO_CODE(digit));
		dummyVal %= ten_base;
		ten_base/=10;
	}
}

void Write_ndigitsval(unsigned int dummyVal, unsigned char ndigits)
{
	unsigned char digit,i;
	unsigned int TestVal=1;

	for (i=1;i<ndigits;i++)
			{TestVal*=10;}

	while(dummyVal<TestVal){TestVal/=10; ndigits--;}

	for (i=1;i<ndigits;i++)
		{PutCommand(CURSOR_MOVE_SHIFT_RIGHT);}

	PutCommand(ENTRY_MODE_DEC_NO_SHIFT);

	while(dummyVal)
	{
		digit = dummyVal%10;
		dummyVal /=10;
		WriteAscii(NUM_TO_CODE(digit));
	}

	PutCommand(ENTRY_MODE_INC_NO_SHIFT);

	for (i=0;i<(ndigits+1);i++)
			{PutCommand(CURSOR_MOVE_SHIFT_RIGHT);}
}

/************************************************
*****     Write HexaDecimal digit on LCD     ****
************************************************/
void Write_HDval(unsigned int dummyVal)
{
	unsigned char HDchar[4] = {0, 0, 0, 0};; // 4 è il massimo di byte che ci servono per
	// una variabile a 32 bit
	unsigned char i=4, p1=0;
	unsigned char Nibbolo; //:-)

	WriteAfter((uint8_t*) "0x");

	while (i)
			{HDchar[i-1]= (dummyVal%16 & 0x0F); // spostiamo nel Nibble Alto
			dummyVal/=16;
			HDchar[i-1] = (HDchar[i-1]) << 4;
			HDchar[i-1] |= (dummyVal%16);
			dummyVal/=16;
			i--;}

	while (4-i)
			{
			Nibbolo = HDchar[i] & 0x0F;
			if(Nibbolo<10) {if(Nibbolo>0) p1=1; if(p1) {WriteAscii(NUM_TO_CODE(Nibbolo)); } }
			else WriteAscii(0x37 + Nibbolo);
			Nibbolo = HDchar[i] & 0xF0;
			Nibbolo = Nibbolo >> 4;
			if(Nibbolo<10) {if(Nibbolo>0) p1=1; if(p1) {WriteAscii(NUM_TO_CODE(Nibbolo)); }  }
			else WriteAscii(0x37 + Nibbolo);
			i++;
			}
}


void Write_HDval2(unsigned int dummyVal)
{
	unsigned int TestVal = 0xFFFFFFF; // 7 nibble è la prima condizione per diminuire le cifre da scrivere
	unsigned char i=0, n_N=8;
	unsigned char Nibbolo; //:-)

	WriteAfter((uint8_t*) "0x");

	while (dummyVal < TestVal) {
	TestVal /= 16;
	n_N--; }

	for (i=1;i<n_N;i++)	{PutCommand(CURSOR_MOVE_SHIFT_RIGHT);}

	PutCommand(ENTRY_MODE_DEC_NO_SHIFT);

	while(dummyVal)
	{
		Nibbolo = dummyVal % 16;
		dummyVal /= 16;
		if(Nibbolo<10) WriteAscii(NUM_TO_CODE(Nibbolo));
		else WriteAscii(0x37 + Nibbolo);
	}

	PutCommand(ENTRY_MODE_INC_NO_SHIFT);

	for (i=0;i<(n_N+1);i++)
			{PutCommand(CURSOR_MOVE_SHIFT_RIGHT);}
}

/****************************************************
*****     Shows the right side of RAM characters ****
*****************************************************/

void DisplayLeft(unsigned char nplaces)
{
	uint8_t i=0;

	DelayMs(1);
	while(i<nplaces)
		{
		PutCommand(DISPLAY_MOVE_SHIFT_LEFT);		// Mostra altri caratteri
		i++;
		}
}

/****************************************************
*****     Shows the left side of RAM characters ****
*****************************************************/

void DisplayRight(unsigned char nplaces)
{
	uint8_t i=0;

	DelayMs(1);
	while(i<nplaces)
		{
		PutCommand(DISPLAY_MOVE_SHIFT_RIGHT);		// Mostra altri caratteri
		i++;
		}
}


// EOF
