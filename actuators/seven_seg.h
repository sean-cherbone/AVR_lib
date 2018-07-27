/* This code is made available for anyone to use but
 * please leave this header intact.
 *
 * Controls common cathode 7-segment LED display.
 * (DON'T FORGET THE RESISTOR)
 *
 * Original code provided by UCR staff, cleaned up
 * and add to by Sean D. Cherbone (scherbone@gmail.com)
 * Enjoy!
 */

#ifndef SEVEN_SEG_H
#define SEVEN_SEG_H

/* USE NOTES:
 * 1.)	None of the provided functions control ports or pins directly.
 *		These functions provide only the binary value to be expressed
 *		on the port or shift register controlling the 7-segment display.
 *		Thus the user is responsible for conveying the function return
 *		values to the appropriate port(s).
 * 2.)	User should note that these functions may provide more
 *		symbols than your project needs.  It may be advisable to
 *		use this full library for testing purposes then pick out
 *		only those functions/symbols that are needed for the
 *		final project in order to reduce memory usage.
 */

// Using the following table:
//		_A_
//	F |		| B
//		_G_
//	E |		| C
//		_D_		. DP
	
// Replace the following shift values with your project
// specific pin configuration per the above layout
#define SEVEN_SEG_A (1 << 2)
#define SEVEN_SEG_B (1 << 3)
#define SEVEN_SEG_C (1 << 5)
#define SEVEN_SEG_D (1 << 6)
#define SEVEN_SEG_E (1 << 7)
#define SEVEN_SEG_F (1 << 1)
#define SEVEN_SEG_G (1 << 0)
#define SEVEN_SEG_DP (1 << 4)

// Use these values for the second argument to get7Seg functions
enum SEVEN_SEG_DOT { DOT_OFF, DOT_ON };

/* Returns the hex value to be expressed on the port
 * controlling a single seven segment display
 * "dot" is a bool value that will turn on the
 * DP LED. Default lights up segment G only.
 * Accepts hex numbers:0x00 through 0x0F */
unsigned char get7SegHex(unsigned char c, int dot);

/* Returns the 8-bit value to display the desired
 * ASCII letter on a single seven segment display.
 * "dot" is a bool value that will turn on the
 * DP LED. Default lights up segment G only.
 * Accepts char values: a-d, A-D, E, F, g-i, G-I,
 * J, l, L, n, o, O, P, r S, u, U, y */
unsigned char get7SegLtr(char c, int dot);

/* Returns the 8-bit value to display the desired
 * ASCII symbol on a single seven segment display.
 * "dot" is a bool value that will turn on the
 * DP LED. Default lights up segment A only.
 * Accepts char values: _, -, [, ], |, '.'. */
unsigned char get7SegSym(char c, int dot);

//----------------------BEGIN FUNCTIONS---------------------------

unsigned char get7SegHex(unsigned char c, int dot) {
	unsigned char retVal = 0;
	c %= 0x10; // Use only the low nibble
	switch (c) {
		case 0:
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_E | SEVEN_SEG_F;
			break;
		case 1:
			retVal = SEVEN_SEG_B | SEVEN_SEG_C;
			break;
		case 2:
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_D |
			SEVEN_SEG_E | SEVEN_SEG_G;
			break;
		case 3:
			retVal = 	SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_G;
			break;
		case 4:
			retVal = SEVEN_SEG_B |	SEVEN_SEG_C | SEVEN_SEG_F |
			SEVEN_SEG_G;
			break;
		case 5:
			retVal = SEVEN_SEG_A | SEVEN_SEG_C | SEVEN_SEG_D |
			SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 6:
			retVal = SEVEN_SEG_A | SEVEN_SEG_C | SEVEN_SEG_D |
			SEVEN_SEG_E | SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 7:
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C;
			break;
		case 8:
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_E | SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 9:
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 0x0A:
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_E | SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 0x0B:
			retVal =  SEVEN_SEG_C | SEVEN_SEG_D | SEVEN_SEG_E |
			SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 0x0C:
			retVal = SEVEN_SEG_A | SEVEN_SEG_D | SEVEN_SEG_E |
			SEVEN_SEG_F;
			break;
		case 0x0D:
			retVal = SEVEN_SEG_B | SEVEN_SEG_C | SEVEN_SEG_D |
			SEVEN_SEG_E | SEVEN_SEG_G;
			break;
		case 0x0E:
			retVal = SEVEN_SEG_A | SEVEN_SEG_D | SEVEN_SEG_E |
			SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 0x0F:
			retVal = SEVEN_SEG_A | SEVEN_SEG_E | SEVEN_SEG_F |
			SEVEN_SEG_G;
			break;
		default:
			retVal = SEVEN_SEG_G;
			break;
	}
	if(dot)
		retVal |= SEVEN_SEG_DP;
	return retVal;
}

unsigned char get7SegLtr(char c, int dot) {
	unsigned char retVal = 0;

	switch (c) {
		case '0':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_E | SEVEN_SEG_F;
			break;
		case '1':
			retVal = SEVEN_SEG_B | SEVEN_SEG_C;
			break;
		case '2':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_D |
			SEVEN_SEG_E | SEVEN_SEG_G;
			break;
		case '3':
			retVal = 	SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_G;
			break;
		case '4':
			retVal = SEVEN_SEG_B |	SEVEN_SEG_C | SEVEN_SEG_F |
			SEVEN_SEG_G;
			break;
		case '5':
			retVal = SEVEN_SEG_A | SEVEN_SEG_C | SEVEN_SEG_D |
			SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case '6':
			retVal = SEVEN_SEG_A | SEVEN_SEG_C | SEVEN_SEG_D |
			SEVEN_SEG_E | SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case '7':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C;
			break;
		case '8':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_E | SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case '9':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 'a':
			retVal = SEVEN_SEG_C | SEVEN_SEG_D | SEVEN_SEG_E |
			SEVEN_SEG_G;
			break;
		case 'A':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_E | SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 'b':
			retVal =  SEVEN_SEG_C | SEVEN_SEG_D | SEVEN_SEG_E |
			SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 'B':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_E | SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 'c':
			retVal = SEVEN_SEG_D | SEVEN_SEG_E | SEVEN_SEG_G;
			break;
		case 'C':
			retVal = SEVEN_SEG_A | SEVEN_SEG_D | SEVEN_SEG_E |
			SEVEN_SEG_F;
			break;
		case 'd':
			retVal = SEVEN_SEG_B | SEVEN_SEG_C | SEVEN_SEG_D |
			SEVEN_SEG_E | SEVEN_SEG_G;
			break;
		case 'D':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_E | SEVEN_SEG_F;
			break;
		case 'E':
			retVal = SEVEN_SEG_A | SEVEN_SEG_D | SEVEN_SEG_E |
			SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 'F':
			retVal = SEVEN_SEG_A | SEVEN_SEG_E | SEVEN_SEG_F |
			SEVEN_SEG_G;
			break;
		case 'g':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 'G':
			retVal = SEVEN_SEG_A | SEVEN_SEG_C | SEVEN_SEG_D |
			SEVEN_SEG_E | SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 'h':
			retVal = SEVEN_SEG_C | SEVEN_SEG_E | SEVEN_SEG_F |
			SEVEN_SEG_G;
			break;
		case 'H':
			retVal = SEVEN_SEG_B | SEVEN_SEG_C | SEVEN_SEG_E |
			SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 'i':
			retVal = SEVEN_SEG_E;
			break;
		case 'I':
			retVal = SEVEN_SEG_E | SEVEN_SEG_F;
			break;
		case 'J':
			retVal = SEVEN_SEG_B | SEVEN_SEG_C | SEVEN_SEG_D |
			SEVEN_SEG_E;
			break;
		case 'l':
			retVal = SEVEN_SEG_E | SEVEN_SEG_F;
			break;
		case 'L':
			retVal = SEVEN_SEG_D | SEVEN_SEG_E | SEVEN_SEG_F;
			break;
		case 'n':
			retVal = SEVEN_SEG_C | SEVEN_SEG_E | SEVEN_SEG_G;
			break;
		case 'o':
			retVal = SEVEN_SEG_C | SEVEN_SEG_D | SEVEN_SEG_E |
			SEVEN_SEG_G;
			break;
		case 'O':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C |
			SEVEN_SEG_D | SEVEN_SEG_E | SEVEN_SEG_F;
			break;
		case 'P':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_E |
			SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 'r':
			retVal = SEVEN_SEG_E | SEVEN_SEG_G;
			break;
		case 'S':
			retVal = SEVEN_SEG_A | SEVEN_SEG_C | SEVEN_SEG_D |
			SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		case 'u':
			retVal = SEVEN_SEG_C | SEVEN_SEG_D | SEVEN_SEG_E;
			break;
		case 'U':
			retVal = SEVEN_SEG_B | SEVEN_SEG_C | SEVEN_SEG_D |
			SEVEN_SEG_E | SEVEN_SEG_F;
			break;
		case 'y':
			retVal = SEVEN_SEG_B | SEVEN_SEG_C | SEVEN_SEG_D |
			SEVEN_SEG_F | SEVEN_SEG_G;
			break;
		default:
			retVal = SEVEN_SEG_G;
			break;
	}
	if(dot)
		retVal |= SEVEN_SEG_DP;
	return retVal;
}

unsigned char get7SegSym(char c, int dot) {
	unsigned char retVal = 0;

	switch (c) {
		case '_':
			retVal = SEVEN_SEG_D;
			break;
		case '-':
			retVal = SEVEN_SEG_G;
			break;
		case '[':
			retVal = SEVEN_SEG_A | SEVEN_SEG_D | SEVEN_SEG_E | SEVEN_SEG_F;
			break;
		case ']':
			retVal = SEVEN_SEG_A | SEVEN_SEG_B | SEVEN_SEG_C | SEVEN_SEG_D;
			break;
		case '|':
			retVal = SEVEN_SEG_B | SEVEN_SEG_C;
			break;
		case '.':
			retVal = SEVEN_SEG_DP;
			break;
		default:
			retVal = SEVEN_SEG_A;
			break;
	}
	if(dot)
		retVal |= SEVEN_SEG_DP;
	return retVal;
}

#endif