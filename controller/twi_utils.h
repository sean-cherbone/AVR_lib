/* This code is made available for anyone to use but
 * please leave this header intact.
 *
 * This code is known to work with the following AVR uCs:
 * atmega1284
 *
 * This code may work with other uCs so check your controller's
 * datasheet to make sure.
 *
 * This library was produced by Sean D. Cherbone (scherbone@gmail.com)
 * Enjoy!
 */
 
/* USE NOTES:
 * 1.)	You must provide pull-up resistors at the end of the
 *		clock and data lines.  See your uCs data sheet for
 *		resistor sizing and attachment.
 *
 * 2.)	In some cases, the hardware that you want to communicate
 *		with may have internal pull-up resistors.  This does not
 *		guarantee that additional resistors are not needed.
 */

/* Typical Master Transmit sequence:
 *	1.)	twi_send_start();
 *	2.)	twi_wait_on_busy();	OR check twi_is_busy(); periodically until not busy
 *	3.)	take action on twi_confirm_status(TWI_MSTR_STAT_START_TRANSMITTED); as necessary
 *	4.) twi_address_slave(<slave_address>, TWI_WRITE)
 *	5.)	twi_wait_on_busy(); OR check twi_is_busy(); periodically until not busy
 *	6.)	take action on twi_confirm_status(TWI_MSTR_STAT_SLA_W_ACK); as necessary
 *	7.)	twi_transmit_uchar(<data>); or some other data
 *	8.)	twi_wait_on_busy(); OR check twi_is_busy(); periodically until not busy
 *	9.)	take action on twi_confirm_status(TWI_MSTR_STAT_DATA_SEND_ACK); as necessary
 *	10.) twi_send_stop();
*/

/* Typical Master Receive sequence:
 *	1.)	twi_send_start();
 *	2.)	twi_wait_on_busy();	OR check twi_is_busy(); periodically until not busy
 *	3.)	take action on twi_confirm_status(TWI_MSTR_STAT_START_TRANSMITTED); as necessary
 *	4.) twi_address_slave(<slave_address>, TWI_READ)
 *	5.)	twi_wait_on_busy(); OR check twi_is_busy(); periodically until not busy
 *	6.)	take action on twi_confirm_status(TWI_MSTR_STAT_SLA_R_ACK); as necessary
 *	7.)	dataVar = twi_receive_uchar(); or some other data type
 *	8.)	twi_wait_on_busy(); OR check twi_is_busy(); periodically until not busy
 *	9.)	take action on twi_confirm_status(TWI_MSTR_STAT_DATA_RECEIVE_ACK); as necessary
 *	10.) twi_send_stop();
*/

#ifndef TWI_UTILS_H
#define TWI_UTILS_H

#include <avr/io.h>

//**************************USER AREA***************************

// Status flags for Master transmitter and receiver
extern const unsigned char TWI_MSTR_STAT_START_TRANSMITTED;
extern const unsigned char TWI_MSTR_STAT_RESTART_TRANSMITTED;
extern const unsigned char TWI_MSTR_STAT_SLA_W_ACK;
extern const unsigned char TWI_MSTR_STAT_SLA_W_NACK;
extern const unsigned char TWI_MSTR_STAT_DATA_SEND_ACK;
extern const unsigned char TWI_MSTR_STAT_DATA_SEND_NACK;
extern const unsigned char TWI_MSTR_STAT_ARBITRATION_LOST;
extern const unsigned char TWI_MSTR_STAT_SLA_R_ACK;
extern const unsigned char TWI_MSTR_STAT_SLA_R_NACK;
extern const unsigned char TWI_MSTR_STAT_DATA_RECEIVE_ACK;
extern const unsigned char TWI_MSTR_STAT_DATA_RECEIVE_NACK;

// Status flags for Slave receiver mode
extern const unsigned char TWI_SLV_STAT_SLA_W_ACK;
extern const unsigned char TWI_SLV_STAT_ARBITRATION_LOST_SLA_W_ACK;
extern const unsigned char TWI_SLV_STAT_GENERAL_CALL_ACK;
extern const unsigned char TWI_SLV_STAT_ARBITRATION_LOST_GEN_CALL_ACK;
extern const unsigned char TWI_SLV_STAT_SLA_W_DATA_RECEIVE_ACK;
extern const unsigned char TWI_SLV_STAT_SLA_W_DATA_RECEIVE_NACK;
extern const unsigned char TWI_SLV_STAT_GEN_CALL_DATA_RECEIVE_ACK;
extern const unsigned char TWI_SLV_STAT_GEN_CALL_DATA_RECEIVE_NACK;
extern const unsigned char TWI_SLV_STAT_STOP_RECEIVED;

// Status flags for Slave transmitter mode
extern const unsigned char TWI_SLV_STAT_SLA_R_ACK;
extern const unsigned char TWI_SLV_STAT_ARBITRATION_LOST_SLA_R_ACK;
extern const unsigned char TWI_SLV_STAT_DATA_SEND_ACK;
extern const unsigned char TWI_SLV_STAT_DATA_SEND_NACK;
extern const unsigned char TWI_SLV_STAT_DATA_RECEIPT_ACK;

// Miscellaneous status flags
extern const unsigned char TWI_STAT_NO_INFO_AVAIL;
extern const unsigned char TWI_STAT_BUS_ERR;

enum TWI_PRESCALERS { TWI_PRESCALER_ONE, TWI_PRESCALER_4TH, TWI_PRESCALER_16TH,
	TWI_PRESCALER_64TH };
	
enum TWI_READ_WRITE { TWI_WRITE, TWI_READ };

/** Set this uC's slave address and set wether or not it should
 *  respond to a "general" call.
 *  @param address					This controller's unique address. Value [0, 127]
 *  @param acknowledgeGeneralCall	0 == don't ack, all other == do ack
 */
void twi_set_myAddress(unsigned char address, unsigned char acknowledgeGeneralCall);

/**
 */
void twi_maskMyAddress(unsigned char mask);

/** @return	1 if line is busy, 0 else
 */
unsigned char twi_isBusy();

/** Performs a busy wait until the line is free
 */
void twi_waitOnBusy();

/** Checks against the status register.
 *  @return 1 == status confirmed, 0 == not confirmed
 */
unsigned char twi_confirmStatus(unsigned char statCode);

/** XXX
 *  @param prescaler	One of the TWI_PRESCALERS enum values
 *  @param bitRate		xxx
 */
void twi_init(int prescaler, unsigned char bitRate);

/** Turn off TWI functionality
 */
void twi_disable();

/** Enable TWI and send "START" signal to get the attention of slave devices.
 */
void twi_sendStart();

/** Enable TWI and send "STOP" signal to tell the slave devices they can rest.
 */
void twi_sendStop();

/** Contact a particular slave and indicate whether you want to read or write from/to it
 *  @param slaveAddress	The unique address of the slave you want to reach. Values [0, 127]
 *  @param read			0 for write request, all else for read
 */
void twi_addressSlave(unsigned char slaveAddress, unsigned char read);

/** 
 */
void twi_get_slvTransmitterStatus();

/**
 */
void twi_transmitUchar(unsigned char data);

/**
 */
unsigned char twi_receiveAck();

/**
 */
unsigned char twi_receiveNack();

//****************************END USER AREA**************************************

const unsigned char TWI_STAT_MASK = 0xF8;

// Status flags for Master transmitter and receiver
const unsigned char TWI_MSTR_STAT_START_TRANSMITTED = 0x08;
const unsigned char TWI_MSTR_STAT_RESTART_TRANSMITTED = 0x10;
const unsigned char TWI_MSTR_STAT_SLA_W_ACK = 0x18;
const unsigned char TWI_MSTR_STAT_SLA_W_NACK = 0x20;
const unsigned char TWI_MSTR_STAT_DATA_SEND_ACK = 0x28;
const unsigned char TWI_MSTR_STAT_DATA_SEND_NACK = 0x30;
const unsigned char TWI_MSTR_STAT_ARBITRATION_LOST = 0x38;
const unsigned char TWI_MSTR_STAT_SLA_R_ACK = 0x40;
const unsigned char TWI_MSTR_STAT_SLA_R_NACK = 0x48;
const unsigned char TWI_MSTR_STAT_DATA_RECEIVE_ACK = 0x50;
const unsigned char TWI_MSTR_STAT_DATA_RECEIVE_NACK = 0x58;

// Status flags for Slave receiver mode
const unsigned char TWI_SLV_STAT_SLA_W_ACK = 0x60;
const unsigned char TWI_SLV_STAT_ARBITRATION_LOST_SLA_W_ACK = 0x68;
const unsigned char TWI_SLV_STAT_GENERAL_CALL_ACK = 0x70;
const unsigned char TWI_SLV_STAT_ARBITRATION_LOST_GEN_CALL_ACK = 0x78;
const unsigned char TWI_SLV_STAT_SLA_W_DATA_RECEIVE_ACK = 0x80;
const unsigned char TWI_SLV_STAT_SLA_W_DATA_RECEIVE_NACK = 0x88;
const unsigned char TWI_SLV_STAT_GEN_CALL_DATA_RECEIVE_ACK = 0x90;
const unsigned char TWI_SLV_STAT_GEN_CALL_DATA_RECEIVE_NACK = 0x98;
const unsigned char TWI_SLV_STAT_STOP_RECEIVED = 0xA0;

// Status flags for Slave transmitter mode
const unsigned char TWI_SLV_STAT_SLA_R_ACK = 0xA8;
const unsigned char TWI_SLV_STAT_ARBITRATION_LOST_SLA_R_ACK = 0xB0;
const unsigned char TWI_SLV_STAT_DATA_SEND_ACK = 0xB8;
const unsigned char TWI_SLV_STAT_DATA_SEND_NACK = 0xC0;
const unsigned char TWI_SLV_STAT_DATA_RECEIPT_ACK = 0xC8;

// Miscellaneous status flags
const unsigned char TWI_STAT_NO_INFO_AVAIL = 0xF8;
const unsigned char TWI_STAT_BUS_ERR = 0x00;

//-----------------FUNCTION DEFINITIONS---------------------

void twi_set_myAddress(unsigned char address, unsigned char acknowledgeGeneralCall) {
	TWAR = address << 1;
	if(acknowledgeGeneralCall)
		TWAR |= (1 << TWGCE);
}

void twi_maskMyAddress(unsigned char mask) {
	TWAMR = mask << 1;
}

unsigned char twi_isBusy() {
	return !(TWCR & (1 << TWINT));
}

void twi_waitOnBusy() {
	while(twi_isBusy())
		continue;
}

unsigned char twi_confirmStatus(unsigned char statCode) {
	return (TWSR & TWI_STAT_MASK) == statCode;
}

void twi_init(int prescaler, unsigned char bitRate) {
	TWSR &= TWI_STAT_MASK;
	switch(prescaler) {
		case TWI_PRESCALER_4TH:
			TWSR |= (1 << TWPS0);
			break;
		case TWI_PRESCALER_16TH:
			TWSR |= (1 << TWPS1);
			break;
		case TWI_PRESCALER_64TH:
			TWSR |= (1 << TWPS0) | (1 << TWPS1);
			break;
		case TWI_PRESCALER_ONE:
		default:
			// no prescaler
	}

	TWBR = bitRate;
}

void twi_disable() {
	TWCR &= ~(1 << TWEN);
}

void twi_sendStart() {
	TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN);
}

void twi_sendStop() {
	TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
}

void twi_addressSlave(unsigned char slaveAddress, unsigned char read) {
	slaveAddress = slaveAddress << 1;
	if(read == TWI_READ)
		slaveAddress |= 0x01;
	TWDR = slaveAddress;
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
}

void twi_get_slvTransmitterStatus() {
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
}

void twi_transmitUchar(unsigned char data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
}

unsigned char twi_receiveAck() {
	unsigned char data = TWDR;
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);
	return data;
}

unsigned char twi_receiveNack() {
	unsigned char data = TWDR;
	TWCR = (1 << TWINT) | (1 << TWEN);
	return data;
}

#endif