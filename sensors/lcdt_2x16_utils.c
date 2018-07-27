/* This code is made available for anyone to use but
 * please leave this header intact.
 *
 * Code is designed for use with AVR atmega32 micro-controller.
 * Controls a GDM1602K type text LCD.
 *
 * This library was produced by Sean D. Cherbone (scherbone@gmail.com)
 * Enjoy!
 */

#ifndef LCDT_2x16_UTILS_C
#define LCDT_2x16_UTILS_C

//*****************************USER MOD AREA***************************************
// Set the following values to match your project setup
// Data pins must be connected P7->D7, P6->D6...
unsigned char *LCDt_DATA_DIR = &DDRC;
unsigned char *LCDt_DATA_PORT = &PORTC;

unsigned char *LCDt_CTRL_DIR = &DDRC;
unsigned char *LCDt_CTRL_PORT = &PORTC;
// use your connection pin number as the shift value
#define LCDt_RS (1 << 2) // Register Select H=Data, L=Instruction
#define LCDt_EN (1 << 3) // Write enable H
//*****************************END USER MOD AREA************************************

// GLOBAL VARIABLES FOR LIBRARY FUNCTION ACCESS ONLY
unsigned char LCDt_8bitMode; // determines how commands and data are sent to LCD
unsigned char LCDt_2lineMode;

// delay needed for data signal stabilize
void LCDt_delayEN() {
	asm("nop");
}

/* Set Control pins and ports for output. */
void LCDt_setCtrlPorts() {
	*LCDt_CTRL_DIR |= LCDt_RS | LCDt_EN;
	*LCDt_CTRL_PORT &= ~(LCDt_RS | LCDt_EN);
}

/* Set Data pins and ports for output. */
void LCDt_setDataPorts() {
	if(LCDt_8bitMode) {
		*LCDt_DATA_DIR = 0xFF;
		*LCDt_DATA_PORT = 0x00;
	}
	else {
		*LCDt_DATA_DIR |= 0xF0;
		*LCDt_DATA_PORT &= 0x0F;
	}
}

/* Sends initial command telling LCD that
 * all future data will be sent via 4-bit
 * serial input. */
void LCDt_send4bitMode() {
	*LCDt_CTRL_PORT &= ~LCDt_RS;
	*LCDt_DATA_PORT |= 0x20;
	*LCDt_CTRL_PORT |= LCDt_EN;
	LCDt_delayEN();
	*LCDt_CTRL_PORT &= ~LCDt_EN;
}

/* Writes a command or data byte to the
 * LCD.  Command write or data write determined
 * by other function. */
void LCDt_write(unsigned char data) {
	static unsigned char i;
	if(LCDt_8bitMode) {
		*LCDt_DATA_PORT = data;
		*LCDt_CTRL_PORT |= LCDt_EN;
		LCDt_delayEN();
		*LCDt_CTRL_PORT &= ~LCDt_EN;
	}
	else {
		for(i = 0; i < 2; ++i) {
			*LCDt_DATA_PORT &= 0x0F;
			*LCDt_DATA_PORT |= 0xF0 & (data << (i * 4));
			*LCDt_CTRL_PORT |= LCDt_EN;
			LCDt_delayEN();
			*LCDt_CTRL_PORT &= ~LCDt_EN;
		}
	}
}

/* Send control command to LCD. */
void LCDt_sendCmd(unsigned char command) {
	*LCDt_CTRL_PORT &= ~LCDt_RS;
	LCDt_write(command);
}

/* Send character display data to LCD. */
void LCDt_sendData(unsigned char data) {
	*LCDt_CTRL_PORT |= LCDt_RS;
	LCDt_write(data);
}

#endif