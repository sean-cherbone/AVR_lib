/* This code is made available for anyone to use but
 * please leave this header intact.
 *
 * Code is designed for use with AVR atmega32 micro-controller.
 * Controls a 16-button, 8-pin matrix keypad.
 *
 * Original "getKeypadInput" code provided by UCR staff,
 * cleaned up and added to by Sean D. Cherbone (scherbone@gmail.com)
 * Enjoy!
 */

#ifndef KEYPAD16_H
#define KEYPAD16_H

#include <avr/io.h>

/*	
	These functions are designed to control and gather data from
	a 16-button, 8-pin matrix keypad.  The code offers a state
	machine that will monitor for keypad presses and will store
	the last value pressed for later retrieval using the getLastKey()
	function.  User must include the code below in the main scheduler
	tasks to use the keypad monitoring state machine.
   
	// User can set the KPM_PERIOD to suit their design needs
	const unsigned long KPM_PERIOD = 100;
	KPM_task.state = -1;
	KPM_task.period = KPM_PERIOD;
	KPM_task.elapsedTime = KPM_PERIOD;
	KPM_task.TickFct = &KPM_Tick;

	Keypad arrangement:
	col  1   2   3   4
	row
	1   1 | 2 | 3 | A
	2   4 | 5 | 6 | B
	3   7 | 8 | 9 | C
	4   * | 0 | # | D
*/

//**************************USER AREA***************************
// Set the following values to match your project setup
static unsigned char *KEYPAD_PORT = &PORTA;
static unsigned char *KEYPAD_PIN = &PINA;
static unsigned char *KEYPAD_DIR = &DDRA;
// Use your connection pin number as the shift value
#define KEYPAD_R1 (1 << 7) // Row 1 pin
#define KEYPAD_R2 (1 << 6) // Row 2 pin
#define KEYPAD_R3 (1 << 5)
#define KEYPAD_R4 (1 << 4)
#define KEYPAD_C1 (1 << 3) // Column 1 pin
#define KEYPAD_C2 (1 << 2) // Column 2 pin
#define KEYPAD_C3 (1 << 1)
#define KEYPAD_C4 (1 << 0)

// Pass one of these into initKeypad & getKeyadInput to determine return type
enum KEYPAD_RETURN_VALS { GET_CHAR, GET_HEX };

//KEYPAD LIBARY FUNCTIONS

// Initialize keypad input and output pins and set the value to
// be returned by the state machine, either hex or char.
// Function should be passed GET_HEX or GET_CHAR as its argument
void initKeypad(int returnValue);

/* Sets keypad PORT and DDR to 0. */
void stopKeypad();

/* Returns the last new button press since last function call.
 * Returns 0xFF if no new button press since last call to this function. */
unsigned char keypad_getLastKey();

/* Scans the attached keypad for input and returns the value of the key pressed.
 * Function should be passed GET_HEX or GET_CHAR as its argument
 * Returns '\0' or 0xFF if no key pressed
 * If multiple keys pressed, returns rightmost-topmost one. */
unsigned char getKeypadInput(int returnValue);
//****************************END USER AREA**************************************

// GLOBAL VARIABLES FOR LIBRARY FUNCTION ACCESS ONLY
static char keypad_KeyValue;
static unsigned char Keypad_newBtnPrs = 0;
static unsigned char keypad_returnHex = 1;

//-----------------FUNCTION DEFINITIONS---------------------

void initKeypad(unsigned char returnValue) {
	// initialize PORT and DDR for input and output
	*KEYPAD_DIR = 0x00;
	*KEYPAD_DIR |= KEYPAD_C1 | KEYPAD_C2 | KEYPAD_C3 | KEYPAD_C4;
	*KEYPAD_PORT = 0x00;
	*KEYPAD_PORT |= KEYPAD_R1 | KEYPAD_R2 | KEYPAD_R3 | KEYPAD_R4;
	// set value to returned by keypad monitoring state machine; hex or char
	keypad_returnHex = returnValue;
}

void stopKeypad() {
	*KEYPAD_PORT = 0x00;
	*KEYPAD_DIR = 0x00;
}

char keypad_getLastKey() {
	if(!Keypad_newBtnPrs) {
		keypad_KeyValue = keypad_returnHex ? 0xFF : '\0';
	}
	Keypad_newBtnPrs = 0;
	return keypad_KeyValue;
}

char getKeypadInput(int returnValue) {
	// Check keys in col 1
	*KEYPAD_PORT = ~KEYPAD_C1; // Enable col 1 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow port to stabilize before checking
	if (!(*KEYPAD_PIN & KEYPAD_R1)) { return returnValue ? 0x01 : '1'; }
	if (!(*KEYPAD_PIN & KEYPAD_R2)) { return returnValue ? 0x04 : '4'; }
	if (!(*KEYPAD_PIN & KEYPAD_R3)) { return returnValue ? 0x07 : '7'; }
	if (!(*KEYPAD_PIN & KEYPAD_R4)) { return returnValue ? 0x0E : '*'; }

	// Check keys in col 2
	*KEYPAD_PORT = ~KEYPAD_C2;
	asm("nop");
	if (!(*KEYPAD_PIN & KEYPAD_R1)) { return returnValue ? 0x02 : '2'; }
	if (!(*KEYPAD_PIN & KEYPAD_R2)) { return returnValue ? 0x05 : '5'; }
	if (!(*KEYPAD_PIN & KEYPAD_R3)) { return returnValue ? 0x08 : '8'; }
	if (!(*KEYPAD_PIN & KEYPAD_R4)) { return returnValue ? 0x00 : '0'; }

	// Check keys in col 3
	*KEYPAD_PORT = ~KEYPAD_C3;
	asm("nop");
	if (!(*KEYPAD_PIN & KEYPAD_R1)) { return returnValue ? 0x03 : '3'; }
	if (!(*KEYPAD_PIN & KEYPAD_R2)) { return returnValue ? 0x06 : '6'; }
	if (!(*KEYPAD_PIN & KEYPAD_R3)) { return returnValue ? 0x09 : '9'; }
	if (!(*KEYPAD_PIN & KEYPAD_R4)) { return returnValue ? 0x0F : '#'; }

	// Check keys in col 4
	*KEYPAD_PORT = ~KEYPAD_C4;
	asm("nop");
	if (!(*KEYPAD_PIN & KEYPAD_R1)) { return returnValue ? 0x0A : 'A'; }
	if (!(*KEYPAD_PIN & KEYPAD_R2)) { return returnValue ? 0x0B : 'B'; }
	if (!(*KEYPAD_PIN & KEYPAD_R3)) { return returnValue ? 0x0C : 'C'; }
	if (!(*KEYPAD_PIN & KEYPAD_R4)) { return returnValue ? 0x0D : 'D'; }

	return returnValue ? 0xFF : '\0'; // default value
}

//--------------------keypad monitoring SM-----------------------------

enum KPM_States { KPM_waitKeyIn, KPM_waitKeyRls };

int KPM_Tick(int state) {
	static unsigned char x = 0xFF;
	
	switch(state) {
		case -1:
			state = KPM_waitKeyIn;
			break;
		case KPM_waitKeyIn:
			if (x == 0xFF) {
				state = KPM_waitKeyIn;
			}
			else { // x != 0xFF
				state = KPM_waitKeyRls;
				keypad_KeyValue = x;
				Keypad_newBtnPrs = 1;
			}
			break;
		case KPM_waitKeyRls:
			if (x != 0xFF) {
				state = KPM_waitKeyRls;
			}
			else { // x == 0xFF
				state = KPM_waitKeyIn;
			}
			break;
		default:
			state = KPM_waitKeyIn;
	}

	switch(state) {
		case KPM_waitKeyIn:
			x = getKeypadInput(keypad_returnHex);
			break;
		case KPM_waitKeyRls:
			x = getKeypadInput(keypad_returnHex);
			break;
		default:
			x = 0xFF;
			break;
	}
	return state;
}

#endif