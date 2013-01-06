/* This code is made available for anyone to use but
 * please leave this header intact.
 *
 * Code is designed for use with AVR atmega32 micro-controller.
 * Controls a GDM1602K type text LCD.
 *
 * This library was produced by Sean D. Cherbone (scherbone@gmail.com)
 * Enjoy!
 */

#ifndef LCDT_TOOLS_2x16_H
#define LCDT_TOOLS_2x16_H

#include <avr/io.h>
#include <string.h>

// set F_CPU to your chip clock frequency. Current: 8MHz
#define F_CPU 8000000UL
#include <math.h>
#include <util/delay.h>

#include "lcdt_2x16_utils.c"

/* USE NOTES:
 * 1.)	The hardware can be wired with either 4 data lines or 8.
 *		4 lines is half as fast as 8 for data transfer.
 *		All data lines must be wired P0->D0, P1->D1...
 *		the R/W pin is not used and can be attached directly
 *		to ground to reduce pin usage.
 * 2.)	Modify the lcdt_2x16_utils.c file to match your project setup.
 * 3.)	Use the USER DATA VALUES below to pass into the
 *		specified functions herein.
 * 4.)	Modify the initSequence array located below to enable
 *		your desired LCD functionality settings. The FUNC_SET
 *		properties are fixed at initialize but the other
 *		settings can be changed at any time.
 * 5.)	LCD requires warm up before receiving data or commands.
 *		See delay requirements below.
 * 6.)	LCD MUST be initialized before use.
 * 7.)	Many of the functions below do not have built in delays
 *		which are required for LCD use.  The user is
 *		responsible for providing the necessary delays
 *		between command functions and data functions that do not
 *		already have built in delays.  if your other CPU tasks
 *		will not provide the needed delay, use the delay functions
 *		provided below for this purpose.  The reason for no built
 *		in delays is this allows the user to perform other tasks
 *		with the CPU instead of simply waisting time in a delay.
 *		This is also useful for concurrent, synchronous state machines.
 *		The "initialize", "writeString" and "writeCharAt" functions
 *		have some built in delay but should be followed by
 *		a delay before calling another LCD function. See delay
 *		data below for required delay time.
 * 8.)	Use delayCMD() for LCD command functions and delayDATA()
 *		for character write functions unless otherwise noted in
 *		function description.
 */

//****************************USER DATA VALUES******************************
// Use these values to pass into the writeString function
enum LCDt_JUSTIFY {LCDt_JUSTIFY_L, LCDt_JUSTIFY_CTR, LCDt_JUSTIFY_R};

// Use these values to pass into the modDisplay function
const unsigned char LCDt_DISPLAY_ON = 0x01;
const unsigned char LCDt_DISPLAY_OFF = 0x00;
const unsigned char LCDt_CURSOR_ON = 0x01;
const unsigned char LCDt_CURSOR_OFF = 0x00;
const unsigned char LCDt_CURSOR_BLINK_ON = 0x01;
const unsigned char LCDt_CURSOR_BLINK_OFF = 0x00;

// Use these values to pass into the modEntry function
const unsigned char LCDt_INC_CURSOR = 0x01;
const unsigned char LCDt_DEC_CURSOR = 0x00;
const unsigned char LCDt_ALLOW_SHIFT = 0x01;
const unsigned char LCDt_NO_SHIFT = 0x00;

// Use these values to pass into the shftCursor & shftDisplay functions
const unsigned char LCDt_SHFT_R = 0x01;
const unsigned char LCDt_SHFT_L = 0x00;
//************************END USER DATA VALUES******************************

// LCD WORK COMMANDS PER MANUFACTURER SPECS
const unsigned char LCDt_CLEAR = 0x01; // clear the lcd screen
const unsigned char LCDt_HOME = 0x02; // return cursor to top left display cell

const unsigned char LCDt_CURSR_SHFT_L = 0x10; // move cursor left
const unsigned char LCDt_CURSR_SHFT_R = 0x14; // move cursor right
const unsigned char LCDt_DISP_SHFT_L = 0x18; // shift display left
const unsigned char LCDt_DISP_SHFT_R = 0x1C; // shift display right

const unsigned char LCDt_CGRAM = 0x40; // add to target CGRAM address location to modify character patterns
const unsigned char LCDt_DDRAM = 0x80; // add to target DDRAM address location to move cursor
const unsigned char LCDt_2ND_LINE = 0x40; // this + LCDt_DDRAM will move the cursor to the start of the second line
const unsigned char LCDt_CHARS_PER_LINE = 40; // max number of characters per line on this LCD

// LCD DELAY PERIODS PER MANUFACTURER SPECS
const unsigned char LCDt_WARMUP_DLY = 40; // time needed for LCD warmup in milliseconds
const unsigned char LCDt_CMD_DLY = 37; // typical max time needed for a command in microseconds
const unsigned char LCDt_DATA_DLY = 40; // typical max time needed for data processing in microseconds
const unsigned short LCDt_CLR_DLY = 1520; // max time needed for screen clear command in microseconds
const unsigned short LCDt_RET_HM_DLY = 1520; // max time needed for return home command in microseconds

// These constants cannot be placed into the LCDt_initSequence array using C programming language
// LCD SETUP COMMANDS PER MANUFACTURER SPECS
const unsigned char LCDt_FUNC_SET1 = 0x20; // set 4-bit interface, 1-line display, 5x8 font
const unsigned char LCDt_FUNC_SET2 = 0x24; // set 4-bit interface, 1-line display, 5x10 font
const unsigned char LCDt_FUNC_SET3 = 0x28; // set 4-bit interface, 2-line display, 5x8 font
const unsigned char LCDt_FUNC_SET4 = 0x30; // set 8-bit interface, 1-line display, 5x8 font
const unsigned char LCDt_FUNC_SET5 = 0x34; // set 8-bit interface, 1-line display, 5x10 font
const unsigned char LCDt_FUNC_SET6 = 0x38; // set 8-bit interface, 2-line display, 5x8 font

const unsigned char LCDt_DISP_MD1 = 0x08; // display off, cursor off, cursor-blink off
const unsigned char LCDt_DISP_MD2 = 0x09; // display off, cursor off, cursor-blink on
const unsigned char LCDt_DISP_MD3 = 0x0A; // display off, cursor on, cursor-blink off
const unsigned char LCDt_DISP_MD4 = 0x0B; // display off, cursor on, cursor-blink on
const unsigned char LCDt_DISP_MD5 = 0x0C; // display on, cursor off, cursor-blink off
const unsigned char LCDt_DISP_MD6 = 0x0D; // display on, cursor off, cursor-blink on
const unsigned char LCDt_DISP_MD7 = 0x0E; // display on, cursor on, cursor-blink off
const unsigned char LCDt_DISP_MD8 = 0x0F; // display on, cursor on, cursor-blink on

const unsigned char LCDt_ENTR_MD1 = 0x04; // decrement cursor left, don't allow shift
const unsigned char LCDt_ENTR_MD2 = 0x05; // decrement cursor left, allow shift
const unsigned char LCDt_ENTR_MD3 = 0x06; // increment cursor right, don't allow shift
const unsigned char LCDt_ENTR_MD4 = 0x07; // increment cursor right, allow shift

//*****************************USER ACCESS FUNCTIONS*******************************
/* input the desired LCD initialization and operation settings
 * in this array.  The input values and their descriptions can
 * be found above.  The values must appear in FUNC, DISP, ENTR
 * order. Current: LCDt_FUNC_SET6, LCDt_DISP_MD5, LCDt_ENTR_MD3. */
const unsigned char LCDt_initSequence[3] = {0x28, 0x0C, 0x06};

/* LCD must be initialized before using any other functions.
 * User must allow 40ms min. warm-up before calling any LCD functions
 * including initialize.
 * This function will initialize all data and control ports and dirs
 * as specified in the LCDt_2x16_utils.c file. */
void LCDt_initialize();

/* Clear all characters from screen/DDRAM.
 * Use delayCLR() for command delay. */
void LCDt_clrScreen();

/* Return cursor to row 0 column 0.
 * Use delayHM() for command delay. */
void LCDt_goHome();

/* Modify LCD display settings. */
void LCDt_modDisplay(unsigned char displayOn, unsigned char cursorOn, unsigned char cursorBlink);

/* Modify LCD behavior for data entry. */
void LCDt_modEntry(unsigned char incCursor, unsigned char allowShft);

/* Move cursor left or right by one column. */
void LCDt_shftCursor(unsigned char direction);

/* Shift display to view other DDRAM values in registers 0-39. */
void LCDt_shftDisplay(unsigned char direction);

/* Move cursor to row and column. */
void LCDt_positionCursor(unsigned char row, unsigned char col);

/* Write a single character to the LCD at the next
 * available space. */
void LCDt_writeChar(char c);

/* Write a single character to the LCD at a
 * specified location. */
void LCDt_writeCharAt(char c, unsigned char row, unsigned char col);

/* Write an 80 character maximum string to LCD display memory.
 * String must be null terminated if less than 80 chars.
 * Use the enumerated justify constants (found above) for the 2nd argument. */
void LCDt_writeString(char str[81], int justify, unsigned char line);

/* Provides the manufacturer specified amount of time delay
 * needed for command to be processed */
void LCDt_delayCMD();

/* Provides the manufacturer specified amount of time delay
 * needed for "return home" command to be processed */
void LCDt_delayHM();

/* Provides the manufacturer specified amount of time delay
 * needed for "screen clear" command to be processed */
void LCDt_delayCLR();

/* Provides the manufacturer specified amount of time delay
 * needed for character data to be processed */
void LCDt_delayDATA();
//**************************END USER ACCESS FUNCTIONS*******************************

//---------------------------FUNCTIONS DEFS-----------------------------

void LCDt_initialize() {
	LCDt_8bitMode = (LCDt_initSequence[0] & 0x10) != 0;
	LCDt_2lineMode = (LCDt_initSequence[0] & 0x08) != 0;
	LCDt_setCtrlPorts();
	LCDt_setDataPorts();
	unsigned char i;
	// initialize LCD for 4bit input
	if(!LCDt_8bitMode) {
		LCDt_send4bitMode();
	}
	// send initialization sequence commands
	for(i = 0; i < 3; ++i) {
		LCDt_delayCMD();
		LCDt_sendCmd(LCDt_initSequence[i]);
	}
	LCDt_delayCMD();
}

void LCDt_clrScreen() {
	LCDt_sendCmd(LCDt_CLEAR);
}

void LCDt_goHome() {
	LCDt_sendCmd(LCDt_HOME);
}

void LCDt_modDisplay(unsigned char displayOn, unsigned char cursorOn, unsigned char cursorBlink) {
	LCDt_sendCmd((LCDt_DISP_MD1 | ((displayOn != 0) << 2) | ((cursorOn != 0) << 1) | (cursorBlink != 0)));
}

void LCDt_modEntry(unsigned char incCursor, unsigned char allowShft) {
	LCDt_sendCmd((LCDt_ENTR_MD1 | ((incCursor != 0) << 1) | (allowShft != 0)));
}

void LCDt_shftCursor(unsigned char direction) {
	if(direction == LCDt_SHFT_R) {
		LCDt_sendCmd(LCDt_CURSR_SHFT_R);
	}
	else {
		LCDt_sendCmd(LCDt_CURSR_SHFT_L);
	}
}

void LCDt_shftDisplay(unsigned char direction) {
	if(direction == LCDt_SHFT_R) {
		LCDt_sendCmd(LCDt_DISP_SHFT_R);
	}
	else {
		LCDt_sendCmd(LCDt_DISP_SHFT_L);
	}
}

void LCDt_positionCursor(unsigned char row, unsigned char col) {
	unsigned char position = LCDt_DDRAM;
	
	if(row && LCDt_2lineMode) {
		position |= LCDt_2ND_LINE;
	}
	
	position |= col % LCDt_CHARS_PER_LINE;
	LCDt_sendCmd(position);
}

void LCDt_writeChar(char c) {
	LCDt_sendData(c);	
}

void LCDt_writeCharAt(char c, unsigned char row, unsigned char col) {
	LCDt_positionCursor(row, col);
	LCDt_delayCMD();
	LCDt_sendData(c);
}

void LCDt_writeString(char str[81], int justify, unsigned char line) {
	unsigned char i, j;
	char strLength;
	
	unsigned char startColumn = 0;
	strLength = strlen(str);
	if(strLength < 16) {
		switch(justify) {
			case LCDt_JUSTIFY_L:
				//startColumn = 0;
				break;
			case LCDt_JUSTIFY_CTR:
				startColumn = 8 - (strLength / 2);
				startColumn -= (strLength % 2) ? 1 : 0;
				break;
			case LCDt_JUSTIFY_R:
				startColumn = 16 - strLength;
				break;
		}
	}
	
	LCDt_positionCursor(line, startColumn);
	LCDt_delayCMD();
	j = line ? 40 : 80;
	for(i = 0; i < j; ++i) {
		if(str[i] == '\0' || i >= strLength)
			break;
		LCDt_writeChar(str[i]);
		LCDt_delayDATA();
	}
}

void LCDt_delayCMD() {
	_delay_us(LCDt_CMD_DLY);
}

void LCDt_delayHM() {
	_delay_us(LCDt_RET_HM_DLY);
}

void LCDt_delayCLR() {
	_delay_us(LCDt_CLR_DLY);
}

void LCDt_delayDATA() {
	_delay_us(LCDt_DATA_DLY);
}

#endif