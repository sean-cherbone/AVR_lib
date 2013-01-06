/* This code is made available for anyone to use but
 * please leave this header intact.
 *
 * Code is designed for use with AVR atmega32 micro-controller.
 * Controls a HC-SR04 type sonic range finder.
 *
 * This library was produced by Sean D. Cherbone (scherbone@gmail.com)
 * Enjoy!
 */

#ifndef SONIC_RANGE_FINDER_H
#define SONIC_RANGE_FINDER_H

#include <avr/io.h>

// set F_CPU to your chip clock frequency. Current: 8MHz
#define F_CPU 8000000UL
#include <math.h>
#include <util/delay.h>

#include "timer_utils.c"

/* USE NOTES:
 * 1.)	Library is designed to be used with an AVR MCU with a
 *		16-bit timer that has "input capture" capabilities.
 *		Check the timer_utils.c file for any changes if you are
 *		using an MCU other than atmega32.
 * 2.)	Initialize range finder before use with initialize function
 *		provided below.
 * 3.)	Range finder requires a minimum of 60 milliseconds between
 *		readings to prevent sound signal interference between reads.
 *		Use HC_SR04_DLY_BTWN_READS for the delay if necessary.
 * 4.)	Use of the provided calibration function is recommended.
 * 5.)	HC-SR04 range finder can take fairly accurate measurements
 *		ranging from approximately 2cm to 4m with a typical
 *		accuracy of 3mm and a measurement range of 15 degrees.
 * 6.)	User will set the timer prescaler value to meet their
 *		project needs and their CPU clock rate. A lower prescaling
 *		value may result in greater measurement accuracy but may
 *		limit measurements to distances less than 4m.
 */

//***********************USER ACCESS AREA*************************

// Set these values to match your project setup
unsigned char *HC_SR04_PORT = &PORTD;
unsigned char *HC_SR04_DIR = &DDRD;
#define HC_SR04_TRIG (1 << PD7)
/* HC_SR04_ECHO must be connected to the Timer1 "input capture"
 * pin: PD6 on atmega32. */

enum DISTANCE_UNITS { MILLIMETER, CENTIMETER, DECIMETER, METER, INCH, FOOT, YARD };
const double SPEED_OF_SOUND = 340.29; // approx. speed of sound in air used to calc distance (meters/second).
const unsigned char HC_SR04_DLY_BTWN_READS = 60; // Minimum time required between distance measurements in milliseconds

/* Initializes required ports and dirs to obtain sonic range reading.
 * Also initializes timer1 for input capture mode with noise filtering.
 * User must call this function before calling getDistance. */
void HC_SR04_initialize();

/* Returns the approximate speed of sound in dry air
 * at 1atm barometric pressure in meters/second. */
double calcSpeedOfSound(double celciusTemp);

/* Takes a measured value from the getDistance function in millimeters
 * and the actual distance from a more precise measurement
 * source in millimeters and adjusts all future getDistance return values
 * accordingly.  Returns the adjustment value in millimeters. */
char HC_SR04_calibrate(unsigned short measuredDistance, unsigned short actualDistance);

/* Returns the distance in "units" from the range finder to an
 * object. Use the enum DISTANCE_UNITS values listed above for the 
 * "units" argument.  "soundSpeed" must be in meters/sec.
 * Use SPEED_OF_SOUND above if no other source.  "tmrPrescaler" is
 * the prescaler that you choose to modulate the timer1 clock speed.
 * Use the enum PRESCALERS values provided in the timer_utils.c file. */
unsigned short HC_SR04_getDistance(unsigned short soundSpeed, int tmrPrescaler, int units);
//************************END USER ACCESS**************************

//GLOBAL VARIABLES FOR LIBRARY ACCESS ONLY
const unsigned char HC_SR04_MIN_TRIG_TIME = 10; // min. time needed for pulse generation in microseconds
char HC_SR04_measure_correction = 5; // Used to adjust measurement based on calibration findings (in millimeters)

//----------------------FUNCTION DEFINITIONS-----------------------

void HC_SR04_initialize() {
	*HC_SR04_DIR |= HC_SR04_TRIG;
	*HC_SR04_PORT &= ~HC_SR04_TRIG;
	initTmr1InCaptPort();
	setTmr1Mode(TMR1_NORMAL_MODE, TMR_COMPARE_NORMAL_MODE, TMR_COMPARE_NORMAL_MODE);
	setTmr1NoiseFilter(ON);
}

/* This function is designed to be used by the getDistance
 * function only and is not intended to be used as a
 * stand alone library function. */
unsigned short HC_SR04_convertUnits(unsigned short value, double soundSpeed, int prescaler, int units) {
	double temp = (double)value * 1000;
	switch(prescaler) {
		case PRESCALER_OFF:
			temp /= F_CPU;
			break;
		case PRESCALER_8TH:
			temp /= F_CPU / 8;
			break;
		case PRESCALER_64TH:
			temp /= F_CPU / 64;
			break;
		case PRESCALER_256TH:
			temp /= F_CPU / 256;
			break;
		case PRESCALER_1024TH:
			temp /= F_CPU / 1024;
			break;
		default:
			temp /= F_CPU; // May produce an incorrect return value
	}
	
	temp *= soundSpeed * 0.5; // time(s) * speadOfSound(m/s) * 1000(mm/m) / 2 = one way distance in millimeters;
	temp += HC_SR04_measure_correction;
	switch(units) {
		case MILLIMETER:
			return (unsigned short)temp;
			break;
		case CENTIMETER:
			return (unsigned short)(temp / 10);
			break;
		case DECIMETER:
			return (unsigned short)(temp / 100);
			break;
		case METER:
			return (unsigned short)(temp / 1000);
			break;
		case INCH:
			return (unsigned short)(temp * 0.0393701);
			break;
		case FOOT:
			return (unsigned short)(temp * 0.00328084);
			break;
		case YARD:
			return (unsigned short)(temp * 0.00109361);
			break;
		default:
			return (unsigned short)(temp / 10); // Centimeters
	}
}

unsigned short HC_SR04_getDistance(unsigned short soundSpeed, int tmrPrescaler, int units) {
	unsigned short start, end;
	// Send out sound pulses
	TIFR = (1 << ICF1); // Clear the timer1 input capture flag
	setTmr1EdgeTrigger(RISING_EDGE);
	setTmr1(0); // Start timer at 0
	setTmr1Prescaler(tmrPrescaler); // Turn on timer
	*HC_SR04_PORT |= HC_SR04_TRIG; // Trigger sound pulses
	_delay_us(HC_SR04_MIN_TRIG_TIME);
	*HC_SR04_PORT &= ~HC_SR04_TRIG; // Turn off trigger
	// Listen for echo
	while(!(TIFR & (1 << ICF1))); // Wait for rising edge
	start = ICR1; // Get start time of duty cycle
	TIFR = (1 << ICF1); // Clear the input capture flag
	setTmr1EdgeTrigger(FALLING_EDGE);
	while(!(TIFR & (1 << ICF1))); // Wait for falling edge
	end = ICR1; // Get end of duty cycle
	stopTmr1();
	
	end -= start; // Get duty cycle time
	return HC_SR04_convertUnits(end, soundSpeed, tmrPrescaler, units);
}

char HC_SR04_calibrate(unsigned short measuredDistance, unsigned short actualDistance) {
	HC_SR04_measure_correction = actualDistance - measuredDistance;
	return HC_SR04_measure_correction;
}

double calcSpeedOfSound(double celciusTemp) {
	return 331.3 + 0.606 * celciusTemp;
}

#endif