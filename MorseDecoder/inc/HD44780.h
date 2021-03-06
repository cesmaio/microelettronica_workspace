/* ###################################################################
**	Filename: HD44780.h
**  Project: 
**	Processor: LPC2103
**	Compiler: gcc version 4.1.1
**	Date: 16/11/2009
**	Abstract:
**		HD44780.h header file
** ###################################################################*/

// clang-format off

#ifndef __HD44780_H
#define __HD44780_H
/* MODULE HD44780 */

#include "LPC8xx.h"
#include "Delay.h"

#define EN				(1<<1)		// P0.01 is EN
#define RS				(1<<0)		// P0.00 is RS
#define DATA_BUS		(15<<16 | 15<<20)	// P0.[23:16]
#define DATA_BUS_LN		(15<<16)	// P0.[19:16]
#define DB4_PIN_SHIFT	20			// DB4 is on P0.20
#define DB4_PIN			(1<<20)		// DB4 is on P0.20
#define DB5_PIN			(1<<21)		// DB5 is on P0.21
#define CTRL_BUS		(EN+RS)
#define FULL_BUS		(CTRL_BUS+DATA_BUS)

#define FULL_BUS_CLEAR	{LPC_GPIO_PORT->CLR[0] = FULL_BUS;}	// Full bus clear
#define FULL_BUS_OUT	{LPC_GPIO_PORT->DIRSET[0] = FULL_BUS;}	// Full bus as output
#define INIT_BUS 		{FULL_BUS_CLEAR;FULL_BUS_OUT;}
#define DEINIT_BUS 		{LPC_GPIO_PORT->DIRCLR0 = DATA_BUS_LN;}

#define ISTR 0	//write to instruction register
#define DATA 1	//write to data register

#define LCD_LINE_LENGHT 80
#define LCD_LINE_VISIBLE 16

//++++  Display COMMANDS and INSTRUCTIONS (see tab. 6 HD44780 AppNote)  +++++++++++++++++++++++
#define DISPLAY_CLEAR				0x01 //	Clears display and set DDRAM address 0
#define RETURN_HOME					0x02 //	DDRAM address 0, cursor origin position

#define ENTRY_MODE_DEC_NO_SHIFT		0x04 //00000100: Entry mode decrement, no display shift
#define ENTRY_MODE_DEC_SHIFT		0x05 //00000101: Entry mode decrement, display shift
#define ENTRY_MODE_INC_NO_SHIFT		0x06 //00000110: Entry mode increment, no display shift
#define ENTRY_MODE_INC_SHIFT		0x07 //00000111: Entry mode increment, display shift

#define DISP_OFF_CUR_OFF_BLINK_OFF	0x08 //00001000: Display off, cursor off, blinking off
#define DISP_OFF_CUR_OFF_BLINK_ON	0x09 //00001001: Display off, cursor off, blinking on
#define DISP_OFF_CUR_ON_BLINK_OFF	0x0A //00001010: Display off, cursor on, blinking off
#define DISP_OFF_CUR_ON_BLINK_ON	0x0B //00001011: Display off, cursor on, blinking on
#define DISP_ON_CUR_OFF_BLINK_OFF	0x0C //00001100: Display on, cursor off, blinking off
#define DISP_ON_CUR_OFF_BLINK_ON	0x0D //00001101: Display on, cursor off, blinking on
#define DISP_ON_CUR_ON_BLINK_OFF	0x0E //00001110: Display on, cursor on, blinking off
#define DISP_ON_CUR_ON_BLINK_ON		0x0F //00001111: Display on, cursor on, blinking on
	
#define CURSOR_MOVE_SHIFT_LEFT		0x10 //00010000: Cursor move, shift to the left
#define CURSOR_MOVE_SHIFT_RIGHT		0x14 //00010100: Cursor move, shift to the right
#define DISPLAY_MOVE_SHIFT_LEFT		0x18 //00011000: Display move, shift to the left
#define DISPLAY_MOVE_SHIFT_RIGHT	0x1C //00011100: Display move, shift to the right

#define FOUR_BIT_ONE_LINE_5x8		0x20 //00100000: 4 bit, 1 line, 5x8 font size
#define FOUR_BIT_ONE_LINE_5x10		0x24 //00101000: 4 bit, 1 line, 5x10 font size
#define FOUR_BIT_TWO_LINE_5x8		0x28 //00101000: 4 bit, 2 line, 5x8 font size
#define FOUR_BIT_TWO_LINE_5x10		0x2C //00101100: 4 bit, 2 line, 5x10 font size
#define EIGHT_BIT_ONE_LINE_5x8		0x30 //00110000: 8 bit, 1 line, 5x8 font size
#define EIGHT_BIT_ONE_LINE_5x10		0x34 //00110100: 8 bit, 1 line, 5x10 font size
#define EIGHT_BIT_TWO_LINE_5x8		0x38 //00111000: 8 bit, 2 line, 5x8 font size
#define EIGHT_BIT_TWO_LINE_5x10		0x3C //00111100: 8 bit, 2 line, 5x10 font size
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define CGRAM_ADDRESS(addr)		(addr = addr|0x40)		 // CGRAM addressing
#define LINE1_HOME		0x80		 // DDRAM addressing
#define LINE2_HOME		0xC0		 // DDRAM addressing

//++++  Functions  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define NUM_TO_CODE(num)		(num+0x30)			 // 0-9 ROM codes
#define CODE_TO_NUM(code)		(code-0x30)         // 0-9 ROM num

void WriteByte(unsigned char rs, unsigned char data_to_LCD);
void InitLCD_4b_1L(void);
void InitLCD_8b_1L(void);
void InitLCD_4b_2L(void); // Use only with 5V separate supply or Charge Pump
void InitLCD_8b_2L(void); // Use only with 5V separate supply or Charge Pump
void WriteAscii(unsigned char symbol);
void PutCommand(unsigned char Command);
void GoToLine(unsigned char line);
void DisplayLeft(unsigned char nplaces);
void DisplayRight(unsigned char nplaces);

void WriteInitial(unsigned char LineOfCharacters[LCD_LINE_VISIBLE]);
void WriteAfter(unsigned char LineOfCharacters[LCD_LINE_LENGHT]);
void WriteAll(unsigned char lineOfCharacters[LCD_LINE_LENGHT]);
void Write_ndigitsval(unsigned int dummyVal, unsigned char ndigits);
void Write_ndigitsval_space(unsigned int dummyVal, unsigned char ndigits);
void Write_2digitsval(unsigned int dummyVal);
void Write_HDval(unsigned int dummyVal);
void Write_HDval2(unsigned int dummyVal);


// # Custom
void writeString(unsigned char lineOfCharacters[LCD_LINE_LENGHT]);
void writeL(unsigned int L, unsigned char lineOfCharacters[LCD_LINE_LENGHT]);
void writeL_replace(unsigned int L, unsigned char lineOfCharacters[LCD_LINE_LENGHT], unsigned char toChange[], unsigned char changeInto[], uint8_t changing_size);
void writeBlinkL(unsigned int L, unsigned int cursor_initial_pos, unsigned char lineOfCharacters[LCD_LINE_LENGHT], unsigned int chars_num, unsigned int blink_n, int blink_time);
unsigned int switchL(unsigned int L);
unsigned int clearL(unsigned int L);
void writeC(unsigned char symbol);
unsigned int slideDisplay(char dir, unsigned int slide_time);
unsigned int writeToXY(unsigned int x, unsigned int y, unsigned char lineOfCharacters[LCD_LINE_LENGHT]);


/* END HD44780 */
#endif /* __HD44780_H*/
