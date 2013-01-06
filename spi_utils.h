/* This code is made available for anyone to use but
 * please leave this header intact.
 *
 * Code is designed for use with AVR micro-controllers (MCUs).  This
 * code was made for the atmega32 MCU but may also work with
 * other AVR MCUs.
 *
 * This library was produced by Sean D. Cherbone (scherbone@gmail.com)
 * Enjoy!
 */

#ifndef SPI_UTILS_C
#define SPI_UTILS_C

/* USE NOTES:
 * 1.)	Code designed for AVR atmega32 MCU; if you are
 *		using a different AVR controller, this code
 *		may need to be modified per that controller's
 *		data sheet.
 * 2.)	SPI can be set up in either slave or master. The
 *		master provides the SCK signal. SPI provides
 *		synchronous transmission only.
 * 3.)	When transmitting to a slave with a Slave Select
 *		(SS) synchronization input, you are responsible
 *		for driving the SS pin low before transmission
 *		and high after transmission is complete.  The
 *		master write functions provided below do not
 *		provide this service, instead use the
 *		"SPIstartTransmit" and "SPIendTransmit" functions.
 * 4.)	Refer to your MCU's data sheet for any
 *		additional functionality.
 */
 
// GLOBAL ENUM VARIABLES TO BE USED AS FORMAL ARGUMENTS TO SPI FUNCTIONS

// Use for master initialization and prescaler functions below.
enum SPI_PRESCALERS { SPI_PRESCALER_HALF, SPI_PRESCALER_QTR, SPI_PRESCALER_8TH,
	SPI_PRESCALER_16TH,	SPI_PRESCALER_32ND, SPI_PRESCALER_64TH, SPI_PRESCALER_128TH };

// Use for the bit order function below.
enum SPI_BIT_ORDER { SPI_MSB_FIRST, SPI_LSB_FIRST };
	
//*****************************USER ACCESS AREA*******************************

/* The SPI pins and ports are fixed but
 * an alternate Slave Select (SS) pin can
 * be used.  Set the values below to match
 * your alternate SS pin, otherwise, set
 * the values to match the default hardware
 * values (Default: PORT/DIR = B, PIN = 4). */
unsigned char *SPI_ALT_PORT = &PORTC;
unsigned char *SPI_ALT_DIR = &DDRC;
#define SPI_ALT_SS (1 << 1) // SS -> Alternate Slave Select

// Check for data write collision
// SPSR & (1 << WCOL)
// Change clock polarity
// SPSR |= (1 << CPOL)
// Shift clock phase
// SPSR |= (1 << CPHA)

/* Initialize SPI unit on MCU for slave mode
 * in order to accept data from another unit. */
void initSlaveSPI();

/* Initialize SPI unit on MCU for master mode
 * in order to transmit serial data. Pass in a value
 * of 1(On) or 0(Off) to either allow or prevent interruption
 * of master mode by exterior master.  Allowing this
 * would change this MCU to slave mode if the SS pin
 * were driven low at any time.  Pass in one of the
 * above prescaler enum values. */
void initMasterSPI(char allowSlaveSwitch, int prescaler);

/* Set the prescaler to adjust the SCK (Slave Clock)
 * speed from the MCU clock speed. Use the enum values
 * above.  SPI prescaler is set to quarter speed
 * by default and half speed is the fastest. */
void setSPIprescaler(int prescaler);

/* This will allow for a global interrupt when
 * the SPIF bit is set.  This occurs when a single
 * byte of data has been transmitted by this MCU
 * as master or an external master.
 * 0(Off), 1(On) */
void enableSPIinterrupt(char enable);

/* SPI interrupt routine.  Use this function to
 * handle an SPI interrupt.
 * ISR(SPI_STC_vect) {
 *
 * }
 */

/* Transmit data start with either MSB or LSB
 * first.  Use enum values above for the argument. */
void SPIbitOrder(int bitOrder);

/* Turn off SPI functionality. */
void disableSPI();

/* Returns positive value if SPI has completed
 * transmission of a previous write, 0 otherwise. */
char SPItransComplete();

/* Lowers the signal on the ALT_SS pin to indicate
 * an incoming transmission to the slave. Call this
 * function before using a "mstrWrite" function to
 * transmit data. */
void SPIstartTransmit();

/* Raises the signal on the ALT_SS pin to indicate
 * transmission complete to the slave. Call this
 * function after using a "mstrWrite" function. */
void SPIendTransmit();

/* Transmit a single unsigned char. Function
 * does not wait for transmission completion. */
void SPImstrWriteUChar(unsigned char data);

/* Transmit a single char.  Function does
 * not wait for transmission completion. */
void SPImstrWriteChar(char c);

/* Transmit a single unsigned short.  Function
 * busy waits for each byte transmission completion. */
void SPImstrWriteUShort(unsigned short data);

/* Transmit a single unsigned long.  Function
 * busy waits for each byte transmission completion. */
void SPImstrWriteULong(unsigned long data);

/* Transmit an unsigned char array of length
 * "size".  Function transmits the 0 element
 * of the array first.  Function busy waits 
 * for each byte transmission completion. */
void SPImstrWriteUCharAry(unsigned char data[], unsigned char size);

/* Transmit a null char terminated char
 * array.  Function busy waits for
 * each byte transmission completion. */
void SPImstrWriteString(char str[]);

/* Receive and return a char transmitted
 * from another source.  Function busy
 * waits for transmission completion. */
char SPIslvReceive();
 
//***************************END USER ACCESS AREA******************************

/* The following variables are chip dependent
 * and cannot be changed from the manufacturer
 * specs for the MOSI, MISO, SCK and SS pins. 
 * This setup is for an Atmega32 chip. */
unsigned char *SPI_PORT = &PORTB;
unsigned char *SPI_DIR = &DDRB;
#define SPI_SCK (1 << 7) // SCK -> Slave Clock
#define SPI_MISO (1 << 6) // MISO -> Master In/Slave Out
#define SPI_MOSI (1 << 5) // MOSI -> Master Out/Slave In
#define SPI_SS (1 << 4) // SS -> Slave Select

void initSlaveSPI() {
	*SPI_DIR &= ~(SPI_MOSI | SPI_SCK | SPI_SS);
	SPCR &= ~(1 << MSTR);
	SPCR |= (1 << SPE);
}

void initMasterSPI(char allowSlaveSwitch, int prescaler) {
	if(allowSlaveSwitch) {
		*SPI_DIR &= ~SPI_SS;
	}
	else {
		*SPI_DIR |= SPI_SS;
		*SPI_ALT_DIR |= SPI_ALT_SS;
	}
	
	*SPI_DIR |= SPI_MOSI | SPI_SCK;
	*SPI_DIR &= ~SPI_MISO;
	*SPI_PORT |= SPI_SS; // Prevent premature data write
	*SPI_ALT_PORT |= SPI_ALT_SS;
	SPCR |= (1 << MSTR) | (1 << SPE);
	setSPIprescaler(prescaler);
}

void setSPIprescaler(int prescaler) {
	SPCR &= ~((1 << SPR1) | (1 << SPR0));
	SPSR &= ~(1 << SPI2X);
	switch(prescaler) {
		case SPI_PRESCALER_HALF:
			SPSR |= (1 << SPI2X);
			break;
		case SPI_PRESCALER_QTR:
			// 1/4 speed
			break;
		case SPI_PRESCALER_8TH:
			SPSR |= (1 << SPI2X);
			SPCR |= (1 << SPR0);
			break;
		case SPI_PRESCALER_16TH:
			SPCR |= (1 << SPR0);
			break;
		case SPI_PRESCALER_32ND:
			SPSR |= (1 << SPI2X);
			SPCR |= (1 << SPR1);
			break;
		case SPI_PRESCALER_64TH:
			SPCR |= (1 << SPR1);
			break;
		case SPI_PRESCALER_128TH:
			SPCR |= (1 << SPR1) | (1 << SPR0);
			break;
		// default: SPI_PRESCALER_QTR
	}
}

void enableSPIinterrupt(char enable) {
	if(enable)
		(SPCR |= (1 << SPIE));
	else
		(SPCR &= ~(1 << SPIE));
}

void SPIbitOrder(int bitOrder) {
	if(bitOrder)
		(SPCR |= (1 << DORD));
	else
		(SPCR &= ~(1 << DORD));
}

void disableSPI() {
	SPCR &= ~(1 << SPE);
}

char SPItransComplete() {
	return (SPSR & (1 << SPIF));
}

void SPIstartTransmit() {
	*SPI_ALT_PORT &= ~SPI_ALT_SS;
}

void SPIendTransmit() {
	*SPI_ALT_PORT |= SPI_ALT_SS;
}

void SPImstrWriteUChar(unsigned char c) {
	SPDR = c;
	while(!(SPSR & (1 << SPIF)))
		continue;
}

void SPImstrWriteChar(char c) {
	SPDR = c;
	while(!(SPSR & (1 << SPIF)))
		continue;
}

void SPImstrWriteUShort(unsigned short s) {
	SPImstrWriteUChar(s >> 8);
	SPImstrWriteUChar(s);
}

void SPImstrWriteULong(unsigned long l) {
	// had to use i - 1 because of bug with i >= 0 for condition
	char i;
	for(i = 4; i >= 1; --i) {
		SPImstrWriteUChar(l >> (8 * (i - 1)));
	}
}

void SPImstrWriteUCharAry(unsigned char data[], unsigned char size) {
	unsigned char i;
	for(i = 0; i < size; ++i) {
		SPImstrWriteUChar(data[i]);
	}
}

void SPImstrWriteString(char str[]) {
	unsigned short i;
	for(i = 0; str[i] != '\0'; ++i) {
		SPImstrWriteChar(str[i]);
	}
}

char SPIslvReceive() {
	while(!(SPSR & (1 << SPIF)))
		continue;
	return SPDR;
}

#endif