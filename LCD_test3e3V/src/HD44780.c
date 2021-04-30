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
#include "Delay.h"
/*****************************************************************
*****  LCD initialization routine  (4 bit interface defined)  ****
*****************************************************************/
unsigned char InitLCD(void)
{
	unsigned char errCode;

	LPC_SYSCON->SYSAHBCLKCTRL[0] |= 1<<6;   // Turn on clock to GPIO0

	INIT_BUS		 //P0.[26:23,16:15] definition: FULL_BUS_CLEAR; FULL_BUS_OUT
	DelayMs(100);	 // Delay a minimum of 100 ms

/*/ This part is on 8 bit interface
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
	LPC_GPIO_PORT->CLR[0] = DB4_PIN; // SETS 4-bit operation
	DelayUs(1);		 // Wait a minimum of 195ns
	LPC_GPIO_PORT->CLR[0] = EN;		 // Clear EN finish write operation
	DelayMs(6);		 // Wait a minimum of 5 ms 10ns*/


	errCode = WriteByte(SEL_IR, DISPLAY_CLEAR);				// Clear display
	DelayMs(1);											// Wait until LCD is free

	errCode = WriteByte(SEL_IR, ENTRY_MODE_INC_SHIFT); 	// Entry mode set cursor direction increment, shift display
	DelayMs(10);											// Wait until LCD is free

//	errCode = WriteByte(SEL_IR, DISP_ON_CUR_ON_BLINK_ON);	// Define display on/off control
//	delayMs(1);											// Wait until LCD is free

	//	errCode = WriteByte(SEL_IR, FOUR_BIT_ONE_LINE_5x10); //Define function set 8 bit interface, 1-line line display, 5x10 font
//	errCode = WriteByte(SEL_IR, FOUR_BIT_TWO_LINE_5x8);	 //Define function set 4 bit interface, 2-line line display, 5x8 font
//	delayMs(1);											 // Wait until LCD is free

	return errCode;
}

/**************************************
*****     Write 2 digit on LCD     ****
***************************************/
void Write_2digitsval(unsigned int dummyVal)
{
	WriteChar(NUM_TO_CODE(dummyVal/10));
	dummyVal %= 10;
	WriteChar(NUM_TO_CODE(dummyVal));
}

/**************************************
*****     Write n digit on LCD     ****
***************************************/
void Write_ndigitsval(unsigned int dummyVal, unsigned char ndigits)
{
	unsigned int ten_base=1, digit, leading_zeroes_flag = 1;
	
	while(--ndigits)
		ten_base*=10;
	
	while(ten_base)
	{
		digit = dummyVal/ten_base;
		if(digit)
			leading_zeroes_flag = 0;
		WriteChar(leading_zeroes_flag&&(ten_base>1)? ' ': NUM_TO_CODE(digit));
		dummyVal %= ten_base;
		ten_base/=10;	
	}
}

/**************************************************
*****    Write a string on LCD visible part    ****
***************************************************/
void WriteVisible(unsigned char LineOfCharacters[LCD_LINE_VISIBLE])
{
	unsigned char i=0 /*, errCode, line=0; number 2 is for 2 lines Display*/;

	GoToLine(1);  DelayUs(100);
	while(LineOfCharacters[i] && i<LCD_LINE_VISIBLE)
	{
//		if(i==LCD_LINE_VISIBLE)		GoToLine(2);
		WriteChar(LineOfCharacters[i]);
		i++;
	}
}

/**************************************************
*****    Write a string on LCD total line      ****
***************************************************/
//void WriteLine(unsigned char lineOfCharacters[LCD_LINE_LENGHT], int line)
void WriteLine(unsigned char lineOfCharacters[LCD_LINE_LENGHT])
{
	unsigned char i=0;

	GoToLine(1);  DelayUs(100);
	while(lineOfCharacters[i] && i<LCD_LINE_LENGHT)
	{
		WriteChar(lineOfCharacters[i]);
		i++;
	}
}

/**************************************
*****    Write a string on LCD     ****
***************************************/
void WriteAfter(unsigned char LineOfCharacters[LCD_LINE_LENGHT])
{
	unsigned char i=0 /*, errCode, line=0*/;

	while(LineOfCharacters[i] && i<LCD_LINE_LENGHT)
	{
		/*errCode = */WriteChar(LineOfCharacters[i]);
		i++;
	}
}


/**************************************
*****   Write a character on LCD   ****
***************************************/
unsigned char WriteChar(unsigned char character)
{
	DelayMs(1);							// Wait until LCD is free
	return WriteByte(SEL_DR, character);	// Write character to DR
}

unsigned char WriteSymbol(uint8_t symbol)
{
	unsigned char symb;

	symb = symbol;
	DelayMs(1);							// Wait until LCD is free
	return WriteByte(SEL_DR, symb);	// Write character to DR
}


/**************************************
*****     Write a byte on LCD      ****
***************************************/
unsigned char WriteByte(unsigned char rs, unsigned char data_to_LCD)
{
	unsigned int dataVal;
	
	if(rs)											
		LPC_GPIO_PORT->SET[0] = RS;								// Set RS (DR operation)
	else
		LPC_GPIO_PORT->CLR[0] = RS;								// Clear RS (IR operation)

	DelayUs(1);									// Wait a minimum of 60ns
	
	LPC_GPIO_PORT->SET[0] = EN;									// Set EN in order to start first write operation
	dataVal = data_to_LCD & 0xF0;				// Copy upper nibble into dataVal
	dataVal = dataVal << (DB4_PIN_SHIFT-4);		// shift left on dataVal to align DB7:DB4 on DATA_BUS
	LPC_GPIO_PORT->PIN[0] = (LPC_GPIO_PORT->PIN[0] & ~DATA_BUS)|dataVal;		// data bus reflects upper nibble
	DelayUs(1);									// Wait a minimum of 195ns
	LPC_GPIO_PORT->CLR[0] = EN;									// Clear EN finish first write operation
	DelayUs(1);									// Wait a minimum of 530ns between writes*/

	LPC_GPIO_PORT->SET[0] = EN;									// Raise EN start second write operation
	dataVal = data_to_LCD & 0x0F;				// Copy lower nibble into dataVal
	dataVal = dataVal << (DB4_PIN_SHIFT);			// shift left on dataVal to align DB4
	LPC_GPIO_PORT->PIN[0] = (LPC_GPIO_PORT->PIN[0] & ~DATA_BUS)|dataVal;		// data bus reflects lower nibble
	DelayUs(1);									// Wait a minimum of 195ns
	LPC_GPIO_PORT->CLR[0] = EN;									// Clear EN finish second write operation
	DelayUs(1);									// Wait a minimum of 30ns
	
	return 0;
}


void DisplayLeft(void)
{
	unsigned char i=0 /*, errCode, line=0*/;

	DelayMs(1);
	while(i<LCD_LINE_VISIBLE)
		{
		PutCommand(DISPLAY_MOVE_SHIFT_LEFT);		// Mostra altri caratteri
		i++;
		}
}


/**************************************
*****    Send a command to LCD     ****
***************************************/
unsigned char PutCommand(unsigned char Command)
{
	DelayMs(1);							// Wait until LCD is free
	return WriteByte(SEL_IR, Command);		// Write character to IR
}

/*-----------------------------------------------------
 | This function cannot be applied to DM0810 (1 line) |
 -----------------------------------------------------*/
unsigned char GoToLine(unsigned char line)
{
	unsigned char address;
	switch(line)
	{
		case 1:
			address = 0x00;
		break;
		case 2:
			address = 0x40;
		break;
		case 3:
			address = 0x10;
		break;
		case 4:
			address = 0x50;
		break;
		default:
		address = 0x00;
		break;
	}
	return PutCommand(DDRAM_ADDRESS(address));
}


// EOF
