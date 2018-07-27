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

#ifndef ADC_UTILS_H
#define ADC_UTILS_H

#include <avr/io.h>

/* USE NOTES:
 * 1.)	Input channels 0 -> 7 coorespond to PINA pins
 *		0 through 7.
 * 2.)	Be sure to use the set functions provided below
 *		as needed before trying to take a reading.
 * 3.)	In order to produce an optimally accurate conversion,
 *		care must be taken to reduce outside signal noise
 *		(see MCU data sheet). The prescaler should be set
 *		to the range specified in the MCU data sheet and certain
 *		parts of the MCU should be put to sleep during the
 *		reading.  If 8-bit reading resolution is adequate, no
 *		prescaling is required.
 * 4.)	The first conversion after ADC functionality
 *		is turned on takes 25 clock cycles with subsequent
 *		conversions requiring 13 cycles, thus if your
 *		conversions are time critical, it may be better
 *		to leave ADC enabled.
 * 5.)	Use the enum values provided below for arguments
 *		to the corresponding library functions.
 */

// GLOBAL ENUM VARIABLES TO BE USED AS FORMAL ARGUMENTS TO ADC FUNCTIONS

/* To be used as the argument to getADCreading
 * function below. */
enum ADC_RESOLUTION { TEN_BIT_RES, EIGHT_BIT_RES };

/* To be used for argument to setADCdiffChannels
 * function below. */
enum ADC_GAIN { x1_ADC_GAIN, x10_ADC_GAIN, x200_ADC_GAIN };

/* To be used for argument to setADCautoTriggerSource
 * function below. */
enum ADC_AUTO_TRIG_SRC { FREE_RUN, ANALOG_COMPARATOR, EXT_INTERRUPT_REQUEST_0, TMR0_CMPR_MATCH,
	TMR0_OVERFLOW, TMR1_CMPR_MATCH_B, TMR1_OVERFLOW, TMR1_CAPTURE_EVENT };

/* To be used for argument to setADCprescaler function below. */
enum ADC_PRESCALERS { ADC_PRESCALER_HALF, ADC_PRESCALER_QTR, ADC_PRESCALER_8TH,
	ADC_PRESCALER_16TH, ADC_PRESCALER_32ND, ADC_PRESCALER_64TH, ADC_PRESCALER_128TH };

/* To be used for argument to setADCrefVltg function below. */
enum ADC_VOLTAGE_REF { AREF_VOLTAGE, AVCC_VOLTAGE, INTERNAL_2_56_VOLTAGE };

//*****************************USER ACCESS AREA*******************************

/* Adjust the ADC clock speed for a conversion.  Use the enum
 * values listed above for the formal argument.  Half speed
 * clock by default.  The ADC clock speed should be between 50kHz
 * and 200kHz to produce max resolution. */
void setADCprescaler(int prescaler);

/* Determines what voltage source will be used for
 * comparison against a single ended analog input.  Use the
 * enum values listed above.  Use AREF pin by default. */
void setADCrefVltg(int refrnc);

/* Choose which pin to check for the analog signal to be
 * converted.  Accepts values between 0 and 7 corresponding
 * to the ADC pin to be used.  Channel 0 by default. */
void setADCsingleEndChannel(unsigned char channel);

/* Choose which pins will be used for comparison and
 * conversion.  Use enum values above for gain and
 * pin numbers used for the channels.  The pin pairs
 * available for this type of conversion are listed
 * in the ADC section of the MCU's data sheet. */
void setADCdiffChannels(int gain, unsigned char posInChannel, unsigned char negInChannel);

/* Select free running mode or a trigger source that
 * will initiate a conversion.  Use 0(Off) 1(On) and
 * the enum values above for the second argument. */
void setADCautoTriggerSource(char onOff, int triggerSource);

/* Allow or disallow an interrupt upon conversion completion.
 * This is useful if using an trigger source for the reading.
 * 0(Off), 1(On) */
void enableADCinterrupt(char enable);

/* Check the ADC interrupt flag:
 * ADCSRA & (1 << ADIF);
 *
 * ADC interrupt routine.  Use this function to
 * handle ADC interrupts.
 * ISR(ADC_vect) {
	
 * }
 */

/* Turn on ADC functionality.  Note that the MCU requires
 * 13 cycles to initialize ADC so the first read takes
 * about twice as long as subsequent reads.
 * 0(Off), 1(On) */
void enableADC(char enable);

/* Begin conversion, busy wait for completion and return
 * converted signal.  Use enum values from above for the
 * argument.  Also use the set functions above as needed.
 * By default, the reading will take a single ended signal
 * from channel 0.  The ADC must be enabled for this
 * function to generate a result. */
short getADCreading(int resolution);

/* Begin a conversion only. This function does not return
 * a converted value, use "retrieveADCreading" to get
 * the last converted signal. This is useful if you
 * can't wait for a conversion or want to use an interrupt
 * handler to capture the conversion when completed. 
 * Use the enum values above for the argument. */
void startADCreading(int resolution);

/* Returns the last converted signal.  May want to use
 * this with "startADCreading". Use the enum values above
 * for the argument.  The argument should match that used
 * to start the conversion. */
short retrieveADCreading(int resolution);

#endif