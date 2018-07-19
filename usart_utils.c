#include "usart_utils.h"

static unsigned short usart_bitMask = 0;

void usart0_set_mode(int usartMode, int parityMode, int bitMode,
	unsigned char useTwoStopBits, unsigned char flipClockPolarity) {
	UCSR0C = 0x00;

	switch(usartMode) {
		case USART_ASYNC_MODE:
			// Asynchronous mode
			break;
		case USART_SYNC_MODE:
			UCSR0C |= (1 << UMSEL00);
			break;
		case USART_MASTER_SPI_MODE:
			UCSR0C |= (1 << UMSEL01) | (1 << UMSEL00);
			break;
	}
	
	switch(parityMode) {
		case USART_PARITY_OFF_MODE:
			// No parity
			break;
		case USART_EVEN_PARITY_MODE:
			UCSR0C |= (1 << UPM01);
			break;
		case USART_ODD_PARITY_MODE:
			UCSR0C |= (1 << UPM00) | (1 << UPM01);
			break;
	}
	
	switch(bitMode) {
		case USART_5_BIT_MODE:
			// 5 bit mode
			usart_bitMask = 0x001F;
			break;
		case USART_6_BIT_MODE:
			UCSR0C |= (1 << UCSZ00);
			usart_bitMask = 0x003F;
			break;
		case USART_7_BIT_MODE:
			UCSR0C |= (1 << UCSZ01);
			usart_bitMask = 0x007F;
			break;
		case USART_8_BIT_MODE:
			UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
			usart_bitMask = 0x00FF;
			break;
		case USART_9_BIT_MODE:
			UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01) | (1 << UCSZ02);
			usart_bitMask = 0x01FF;
			break;
	}
	
	if(useTwoStopBits)
		UCSR0C |= 1 << USBS0;
	if(usartMode != USART_ASYNC_MODE && flipClockPolarity)
		UCSR0C |= 1 << UCPOL0;
}

void usart_start(unsigned short baud, unsigned char enableRx, unsigned char enableTx) {
	UBRRH0 = (unsigned char)(rate >> 8);
	UBRRL0 = (unsigned char)rate;
	
	UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));
	UCSR0A &= ~(1 << UDRE0);
	if(enableRx)
		UCSR0B |= (1 << RXEN0);
	if(enableTx) {
		UCSR0B |= (1 << TXEN0);
		UCSR0A |= (1 << UDRE0);
	}
}

void usart_stop() {
	UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));
}

//////////////////////////////////////////////////////////////////////////
// Transmitters
//////////////////////////////////////////////////////////////////////////

void usart0_transmit_bits(unsigned short data) {
	while (!(UCSR0A & (1 << UDRE0)))
	
	if(bitMask & 0x0100) {
		UCSR0B &= ~(1 << TXB8);
		if(data & 0x0100) UCSR0B |= (1 << TXB8);
	}
	UDR0 = (unsigned char)(data & usart_bitMask);
}

void usart0_transmit_uchar(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)))
		continue;
	UDR0 = data;
}

void usart0_transmit_char(char data) {
	while (!(UCSR0A & (1 << UDRE0)))
		continue;
	UDR0 = data;
}

void usart0_transmit_ushort(unsigned short data) {
	usart0_transmit_uchar(data);
	usart0_transmit_uchar(data >> 8);
}

void usart0_transmit_short(short data) {
	usart0_transmit_uchar(data);
	usart0_transmit_uchar(data >> 8);
}

void usart0_transmit_ulong(unsigned long data) {
	unsigned char i;
	for(i = 0; i < 4; ++i)
		usart0_transmit_uchar(data >> (i * 8));
}

void usart0_transmit_long(long data) {
	unsigned char i;
	for(i = 0; i < 4; ++i)
		usart0_transmit_uchar(data >> (i * 8));
}

void usart0_transmit_ucharAry(unsigned char ary[], unsigned char size) {
	unsigned char i;
	for(i = 0; i < size; ++i)
		usart0_transmit_uchar(ary[i]);
}

void usart0_transmit_string(char str[]) {
	char *it;
	for(it = str; *it != '\0'; ++it)
		usart0_transmit_char(*it);
	usart0_transmit_char('\0');
}

//////////////////////////////////////////////////////////////////////////
// Receivers
//////////////////////////////////////////////////////////////////////////

void usart0_transmit_bits(unsigned short data, unsigned short bitMask) {

}

unsigned char usart0_receive_uchar() {
	while (!(UCSR0A & (1 << RXC0)))
		continue;
	return UDR0;
}

char usart0_receive_char() {
	while (!(UCSR0A & (1 << RXC0)))
		continue;
	return UDR0;
}

unsigned short usart0_receive_ushort() {
	unsigned short buf = 0;
	buf |= usart0_receive_uchar() << 8;
	buf |= usart0_receive_uchar();
	return buf;
}

short usart0_receive_short() {
	unsigned short buf = 0;
	buf |= usart0_receive_uchar() << 8;
	buf |= usart0_receive_uchar();
	return buf;
}

unsigned long usart0_receive_ulong() {
	unsigned long buf = 0;
	char i;
	for(i = 3; i >= 0; --i)
		buf |= usart0_receive_uchar() << (i * 8);
	return buf;
}

long usart0_receive_long() {
	long buf = 0;
	char i;
	for(i = 3; i >= 0; --i)
		buf |= usart0_receive_uchar() << (i * 8);
	return buf;
}

void usart0_receive_ucharAry(unsigned char *buf, unsigned char bufSize) {
	unsigned char i;
	for(i = 0; i < bufSize; ++i)
		buf[i] = usart_receive_char();
}

void usart0_receive_string(char *buf, unsigned char bufSize) {
	char c;
	unsigned char i;
	for(i = 0; i < bufSize-1; ++i) {
		c = usart0_receive_char();
		if(c == '\0')
			break;
		buf[i] = c;
	}
	buf[i] = '\0';
}

unsigned int USART_Receive( void )
{
unsigned char status, resh, resl;
/* Wait for data to be received */
while ( !(UCSRnA & (1<<RXCn)) )
;
/* Get status and 9th bit, then data */
/* from buffer */
status = UCSRnA;
resh = UCSRnB;
resl = UDRn;
/* If error, return -1 */
if ( status & (1<<FEn)|(1<<DORn)|(1<<UPEn) )
return -1;
/* Filter the 9th bit, then return */
resh = (resh >> 1) & 0x01;
return ((resh << 8) | resl);
}
