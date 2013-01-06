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

#ifndef TIMER_UTILS_C
#define TIMER_UTILS_C

#include <avr/io.h>
#include <avr/iom32.h>
#include <avr/interrupt.h>

/* USE NOTES:
 * 1.)	This library was designed for the atmega32 MCU and
 *		may or may not work directly with other AVR
 *		micro controllers.
 * 2.)	See the timer mode notes at the top of each timer#
 *		code section.
 */

// GLOBAL ENUM VARIABLES TO BE USED AS FORMAL ARGUMENTS TO TIMER FUNCTIONS

// Use for all timer prescaler arguments
enum TIMER_PRESCALERS { TMR_PRESCALER_OFF, TMR_PRESCALER_8TH, TMR_PRESCALER_64TH,
	TMR_PRESCALER_256TH, TMR_PRESCALER_1024TH, TMR_PRESCALER_EXT_FALL, TMR_PRESCALER_EXT_RISE };

// Use as compareOutMode argument(s) for all setTmrMode functions
enum TIMER_COMPARE_MODES { TMR_COMPARE_NORMAL_MODE, TMR_COMPARE_TOGGLE_MODE,
	TMR_COMPARE_CLEAR_MODE, TMR_COMPARE_SET_MODE };

// Use for first argument to setTmrMode for timers 0 and 2
enum TIMER02_MODES { TMR02_NORMAL_MODE, TMR02_PHASE_CORRECT_PWM_MODE,
	TMR02_CTC_MODE, TMR02_FAST_PWM_MODE };

// Use for argument to enableTmrInterrupts for timers 0 and 2
enum TIMER02_INTERRUPTS { TMR02_COMPARE_INTERRUPT, TMR02_OVERFLOW_INTERRUPT};

// Use for first argument to setTmr1Mode function
enum TIMER1_MODES { TMR1_NORMAL_MODE, TMR1_CTC_OCR_MODE, TMR1_CTC_ICR_MODE,
	TMR1_FAST_PWM_8BIT_MODE, TMR1_FAST_PWM_9BIT_MODE, TMR1_FAST_PWM_10BIT_MODE,
	TMR1_FAST_PWM_OCR_MODE, TMR1_FAST_PWM_ICR_MODE,
	TMR1_PHASE_CORRECT_PWM_8BIT_MODE, TMR1_PHASE_CORRECT_PWM_9BIT_MODE, TMR1_PHASE_CORRECT_PWM_10BIT_MODE,
	TMR1_PHASE_CORRECT_PWM_OCR_MODE, TMR1_PHASE_CORRECT_PWM_ICR_MODE,
	TMR1_PHASE_FREQ_CORRECT_PWM_OCR_MODE, TMR1_PHASE_FREQ_CORRECT_PWM_ICR_MODE };

// Use for argument to setTmr1EdgeTrigger function
enum TIMER1_EDGE_TRIGGERS { RISING_EDGE, FALLING_EDGE };

// Use for argument to enableTmr1Interrupts function
enum TIMER1_INTERRUPTS { TMR1_INPUT_CAPTURE_INTERRUPT, TMR1_COMPARE_A_INTERRUPT,
	TMR1_COMPARE_B_INTERRUPT, TMR1_OVERFLOW_INTERRUPT };

/* Clears any prescaler settings for timers 0 and 1 such
 * that the timer operates at the system clock speed. */
void resetTmr01Prescalers() {
	SFIOR |= (1 << PSR10);
}

//------------------------TIMER 0-------------------------

unsigned char *TMR0_OUT_PORT = &PORTB;
unsigned char *TMR0_OUT_DIR = &DDRB;
#define TMR0_OUT_COMP_MATCH_PIN (1 << PB3)

unsigned char *TMR0_IN_CNTR_PORT = &PORTB;
unsigned char *TMR0_IN_CNTR_DIR = &DDRB;
#define TMR0_EXT_CNTR_INPUT_PIN (1 << PB0)

/* Insert the following code snippets into your own
 * functions as needed.*/
 
// Get status of output compare match
// TIFR & (1 << OCF0);
// Get status of timer overflow flag
// TIFR & (1 << TOV0);

// Clear OCF0 flag
// TIFR = (1 << OCF0);
// Clear TOV0 flag
// TIFR = (1 << TOV0);

/* timer0 interrupt handlers
ISR(TIMER0_COMP_vect) {
	// handle interrupt
}

ISR(TIMER0_OVF_vect) {
	// handle interrupt
}
*/

/* Sets output dir and port for compare match out. */
void initTmr0OutPort() {
	*TMR0_OUT_DIR |= TMR0_OUT_COMP_MATCH_PIN;
	*TMR0_OUT_PORT &= ~TMR0_OUT_COMP_MATCH_PIN;
}

/* Sets input dir and port for external count input. */
void initTmr0InPort() {
	*TMR0_IN_CNTR_DIR &= ~TMR0_EXT_CNTR_INPUT_PIN;
	*TMR0_IN_CNTR_PORT &= ~TMR0_EXT_CNTR_INPUT_PIN;
}

/* Determines functionality of timer0.  See AVR data sheet
 * for details on timer functions. Use enum TIMER02_MODES
 * and enum TIMER_COMPARE_MODES values for arguments. */
void setTmr0Mode(int timerMode, int compareOutMode) {
	TCCR0 &= ~((1 << COM00) | (1 << COM01) | (1 << WGM00) | (1 << WGM01));
	switch(timerMode) {
		case TMR02_NORMAL_MODE:
			// Normal Mode
			break;
		case TMR02_PHASE_CORRECT_PWM_MODE:
			TCCR0 |= (1 << WGM00);
			break;
		case TMR02_CTC_MODE:
			TCCR0 |= (1 << WGM01);
			break;
		case TMR02_FAST_PWM_MODE:
			TCCR0 |= (1 << WGM00) | (1 << WGM01);
			break;
		// default normal mode
	}
	
	switch(compareOutMode) {
		case TMR_COMPARE_NORMAL_MODE:
			// Normal Mode
			break;
		case TMR_COMPARE_TOGGLE_MODE:
			TCCR0 |= (1 << COM00);
			break;
		case TMR_COMPARE_CLEAR_MODE:
			TCCR0 |= (1 << COM01);
			break;
		case TMR_COMPARE_SET_MODE:
			TCCR0 |= (1 << COM00) | (1 << COM01);
			break;
		// default normal mode
	}
}

/* Modifies the timer/counter speed by dividing the system
 * clock speed. Use enum PRESCALERS values for the argument.
 * Timer starts as soon as prescaler is set. */
void setTmr0Prescaler(int prescaler) {
	TCCR0 &= ~((1 << CS00) | (1 << CS01) | (1 << CS02));
	switch(prescaler) {
		case TMR_PRESCALER_OFF:
			TCCR0 |= (1 << CS00);
			break;
		case TMR_PRESCALER_8TH:
			TCCR0 |= (1 << CS01);
			break;
		case TMR_PRESCALER_64TH:
			TCCR0 |= (1 << CS01) | (1 << CS00);
			break;
		case TMR_PRESCALER_256TH:
			TCCR0 |= (1 << CS02);
			break;
		case TMR_PRESCALER_1024TH:
			TCCR0 |= (1 << CS02) | (1 << CS00);
			break;
		case TMR_PRESCALER_EXT_FALL:
			TCCR0 |= (1 << CS02) | (1 << CS01);
			break;
		case TMR_PRESCALER_EXT_RISE:
			TCCR0 |= (1 << CS02) | (1 << CS01) | (1 << CS00);
			break;
		default:
			TCCR0 |= (1 << CS00); // Prescaler off
	}
}

/* Set the TOP value at which the timer0 output
 * compare flag will be set and the timer will reset. */
void setTmr0CompareVal(unsigned char time) {
	OCR0 = time;
}

/* Forces the output compare pin to act as though
 * an actual compare occurred but does not affect
 * the output compare flag or interrupt if enabled. */
void forceCompareTmr0() {
	TCCR0 |= (1 << FOC0);
}

/* Stops timer0 but does not reset it. */
void stopTmr0() {
	TCCR0 &= ~((1 << CS00) | (1 << CS01) | (1 << CS02));
}

/* Sets the time at which the timer will start counting.
 * Temporarily disables global interrupts. */
void setTmr0(unsigned char time) {
	unsigned char sreg;
	
	sreg = SREG;
	SREG &= 0x7F;
	TCNT0 = time;
	SREG = sreg;
}

/* Returns the current value in the timer0 register.
 * Temporarily disables global interrupts. */
unsigned char readTmr0() {
	unsigned char sreg, temp;
	
	sreg = SREG;
	SREG &= 0x7F;
	temp = TCNT0;
	SREG = sreg;
	return temp;
}

/* Enable or disable any of the available interrupts
 * for timer0.  Use the enum TIMER02_INTERRUPTS values
 * for the interrupt argument. (0)disable, (1)enable*/
void enableTmr0Interrupts(int interrupt, char enable) {
	switch(interrupt) {
		case TMR02_COMPARE_INTERRUPT:
			if(enable)
				TIMSK |= (1 << OCIE0);
			else
				TIMSK &= ~(1 << OCIE0);
			break;
		case TMR02_OVERFLOW_INTERRUPT:
			if(enable)
				TIMSK |= (1 << TOIE0);
			else
				TIMSK &= ~(1 << TOIE0);
			break;
		// default do nothing
	}
}

//------------------------TIMER 1-------------------------

unsigned char *TMR1_OUT_PORT = &PORTD;
unsigned char *TMR1_OUT_DIR = &DDRD;
#define TMR1_OUT_COMP_MATCH_A_PIN (1 << PD5)
#define TMR1_OUT_COMP_MATCH_B_PIN (1 << PD4)

unsigned char *TMR1_IN_CAPT_PORT = &PORTD;
unsigned char *TMR1_IN_CAPT_DIR = &DDRD;
#define TMR1_IN_CAPT_PIN (1 << PD6)

unsigned char *TMR1_IN_CNTR_PORT = &PORTB;
unsigned char *TMR1_IN_CNTR_DIR = &DDRB;
#define TMR1_EXT_CNTR_INPUT_PIN (1 << PB1)

/* Insert the following code snippets into your own
 * functions as needed.*/

// Get status of input capture flag
// TIFR & (1 << ICF1)
// Get status of output compare match A flag
// TIFR & (1 << OCF1A)
// Get status of output compare match B flag
// TIFR & (1 << OCF1B)
// Get status of timer overflow flag
// TIFR & (1 << TOV1)

// Clear ICF1 flag
// TIFR = (1 << ICF1);
// Clear OCF1A flag
// TIFR = (1 << OCF1A); 
// Clear OCF1B flag
// TIFR = (1 << OCF1B);
// Clear TOV1 flag
// TIFR = (1 << TOV1);

/* timer1 interrupt handlers
ISR(TIMER1_CAPT_vect) {
	// handle interrupt
}

ISR(TIMER1_COMPA_vect) {
	// handle interrupt
}

ISR(TIMER1_COMPB_vect) {
	// handle interrupt
}

ISR(TIMER1_OVF_vect) {
	// handle interrupt
}
*/

/* Sets output dir and port for compare_A match out. */
void initTmr1OutAPort() {
	*TMR1_OUT_DIR |= TMR1_OUT_COMP_MATCH_A_PIN;
	*TMR1_OUT_PORT &= ~TMR1_OUT_COMP_MATCH_A_PIN;
}

/* Sets output dir and port for compare_B match out. */
void initTmr1OutBPort() {
	*TMR1_OUT_DIR |= TMR1_OUT_COMP_MATCH_B_PIN;
	*TMR1_OUT_PORT &= ~TMR1_OUT_COMP_MATCH_B_PIN;
}

/* Sets input capture dir and port. */
void initTmr1InCaptPort() {
	*TMR1_IN_CAPT_DIR &= ~TMR1_IN_CAPT_PIN;
	*TMR1_IN_CAPT_PORT &= ~TMR1_IN_CAPT_PIN;
}

/* Sets input dir and port for external count input. */
void initTmr1InCntrPort() {
	*TMR1_IN_CNTR_DIR &= ~TMR1_EXT_CNTR_INPUT_PIN;
	*TMR1_IN_CNTR_PORT &= ~TMR1_EXT_CNTR_INPUT_PIN;
}

/* Determines functionality of timer1.  See AVR data sheet
 * for details on timer functions. Use enum TIMER1_MODES
 * and enum TIMER_COMPARE_MODES values for arguments. */
void setTmr1Mode(int timerMode, int compareOutModeA, int compareOutModeB) {
	TCCR1A &= ~((1 << COM1A0) | (1 << COM1A1) | (1 << COM1B0) | (1 << COM1B1) | (1 << WGM10) | (1 << WGM11));
	TCCR1B &= ~((1 << WGM12) | (1 << WGM13));
	switch(timerMode) {
		case TMR1_NORMAL_MODE:
			// Keep all COM and WGM bits low
			break;
		case TMR1_CTC_OCR_MODE:
			TCCR1B |= (1 << WGM12);
			break;
		case TMR1_CTC_ICR_MODE:
			TCCR1B |= (1 << WGM12) | (1 << WGM13);
			break;
		case TMR1_FAST_PWM_8BIT_MODE:
			TCCR1A |= (1 << WGM10);
			TCCR1B |= (1 << WGM12);
			break;
		case TMR1_FAST_PWM_9BIT_MODE:
			TCCR1A |= (1 << WGM11);
			TCCR1B |= (1 << WGM12);
			break;
		case TMR1_FAST_PWM_10BIT_MODE:
			TCCR1A |= (1 << WGM10) | (1 << WGM11);
			TCCR1B |= (1 << WGM12);
			break;
		case TMR1_FAST_PWM_OCR_MODE:
			TCCR1A |= (1 << WGM11);
			TCCR1B |= (1 << WGM13) | (1 << WGM12);
			break;
		case TMR1_FAST_PWM_ICR_MODE:
			TCCR1A |= (1 << WGM10) | (1 << WGM11);
			TCCR1B |= (1 << WGM12) | (1 << WGM13);
			break;
		case TMR1_PHASE_CORRECT_PWM_8BIT_MODE:
			TCCR1A |= (1 << WGM10);
			break;
		case TMR1_PHASE_CORRECT_PWM_9BIT_MODE:
			TCCR1A |= (1 << WGM11);
			break;
		case TMR1_PHASE_CORRECT_PWM_10BIT_MODE:
			TCCR1A |= (1 << WGM10) | (1 << WGM11);
			break;
		case TMR1_PHASE_CORRECT_PWM_OCR_MODE:
			TCCR1A |= (1 << WGM10) | (1 << WGM11);
			TCCR1B |= (1 << WGM13);
			break;
		case TMR1_PHASE_CORRECT_PWM_ICR_MODE:
			TCCR1A |= (1 << WGM11);
			TCCR1B |= (1 << WGM13);
			break;
		case TMR1_PHASE_FREQ_CORRECT_PWM_OCR_MODE:
			TCCR1A |= (1 << WGM10);
			TCCR1B |= (1 << WGM13);
			break;
		case TMR1_PHASE_FREQ_CORRECT_PWM_ICR_MODE:
			TCCR1B |= (1 << WGM13);
			break;
		// default normal mode
	}
	
	switch(compareOutModeA) {
		case TMR_COMPARE_NORMAL_MODE:
			// Normal mode
			break;
		case TMR_COMPARE_TOGGLE_MODE:
			TCCR1A |= (1 << COM1A0);
			break;
		case TMR_COMPARE_CLEAR_MODE:
			TCCR1A |= (1 << COM1A1);
			break;
		case TMR_COMPARE_SET_MODE:
			TCCR1A |= (1 << COM1A0) | (1 << COM1A1);
			break;
		// default normal mode
	}
	
	switch(compareOutModeB) {
		case TMR_COMPARE_NORMAL_MODE:
			// Normal mode
			break;
		case TMR_COMPARE_TOGGLE_MODE:
			TCCR1A |= (1 << COM1B0);
			break;
		case TMR_COMPARE_CLEAR_MODE:
			TCCR1A |= (1 << COM1B1);
			break;
		case TMR_COMPARE_SET_MODE:
			TCCR1A |= (1 << COM1B0) | (1 << COM1B1);
			break;
		// default normal mode
	}
}

/* Modifies the timer/counter speed by dividing the system
 * clock speed. Use enum PRESCALERS values for the argument.
 * Timer starts as soon as prescaler is set. */
void setTmr1Prescaler(int prescaler) {
	TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));
	switch(prescaler) {
		case TMR_PRESCALER_OFF:
			TCCR1B |= (1 << CS10);
			break;
		case TMR_PRESCALER_8TH:
			TCCR1B |= (1 << CS11);
			break;
		case TMR_PRESCALER_64TH:
			TCCR1B |= (1 << CS10) | (1 << CS11);
			break;
		case TMR_PRESCALER_256TH:
			TCCR1B |= (1 << CS12);
			break;
		case TMR_PRESCALER_1024TH:
			TCCR1B |= (1 << CS10) | (1 << CS12);
			break;
		case TMR_PRESCALER_EXT_FALL:
			TCCR1B |= (1 << CS11) | (1 << CS12);
			break;
		case TMR_PRESCALER_EXT_RISE:
			TCCR1B |= (1 << CS10) | (1 << CS11) | (1 << CS12);
			break;
		default:
			TCCR1B |= (1 << CS10); // Prescaler off
	}
}

/* Set the TOP value at which the timer1 output
 * compare flag will be set and the timer will reset. 
 * Pass in 'A' or 'B' for the second argument. */
void setTmr1CompareVal(unsigned short time, char comparatorAB) {
	switch(comparatorAB) {
		case 'A':
			OCR1A = time;
			break;
		case 'B':
			OCR1B = time;
			break;
		// default do nothing
	}
}

/* Forces the output compare pin to act
 * as though an actual compare occurred but does not
 * affect the output compare flag or interrupt if enabled.
 * Pass in 'A' or 'B' for the argument. */
void forceCompareTmr1(char compareAB) {
	switch(compareAB) {
		case 'A':
			TCCR1A |= (1 << FOC1A);
			break;
		case 'B':
			TCCR1A |= (1 << FOC1B);
			break;
		// default do nothing
	}
}

/* Stops timer1 but does not reset it. */
void stopTmr1() {
	TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));
}

/* Sets the time at which the timer will start counting.
 * Temporarily disables global interrupts. */
void setTmr1(unsigned short time) {
	unsigned char sreg;

	sreg = SREG;
	SREG &= 0x7F;
	TCNT1 = time;
	SREG = sreg;
}

/* Returns the current value in the timer1 register.
 * Temporarily disables global interrupts. */
unsigned short readTmr1() {
	unsigned char sreg;
	unsigned short temp;
	
	sreg = SREG;
	SREG &= 0x7F;
	temp = TCNT1;
	SREG = sreg;
	
	return temp;
}

/* Enable or disable any of the available interrupts
 * for timer1.  Use the enum TIMER1_INTERRUPTS values
 * for the interrupt argument. (0)disable, (1)enable */
void enableTmr1Interrupts(int interrupt, char enable) {
	switch(interrupt) {
		case TMR1_INPUT_CAPTURE_INTERRUPT:
			if(enable)
				TIMSK |= (1 << TICIE1);
			else
				TIMSK &= ~(1 << TICIE1);
			break;
		case TMR1_COMPARE_A_INTERRUPT:
			if(enable)
				TIMSK |= (1 << OCIE1A);
			else
				TIMSK &= ~(1 << OCIE1A);
			break;
		case TMR1_COMPARE_B_INTERRUPT:
			if(enable)
				TIMSK |= (1 << OCIE1B); 
			else
				TIMSK &= ~(1 << OCIE1B);
			break;
		case TMR1_OVERFLOW_INTERRUPT:
			if(enable)
				TIMSK |= (1 << TOIE1);
			else
				TIMSK &= ~(1 << TOIE1);
			break;
		// default do nothing
	}
}

/* Sets either the rising or falling edge event as the
 * trigger for the event capture timer mode. */
void setTmr1EdgeTrigger(int edge) {
	switch(edge) {
		case RISING_EDGE:
			TCCR1B |= (1 << ICES1);
			break;
		case FALLING_EDGE:
			TCCR1B &= ~(1 << ICES1);
			break;
		default:
			TCCR1B |= (1 << ICES1); // Rising edge
	}
}

/* Enables or disables the hardware noise filter for use
 * in input capture mode. Causes a 4 clock cycle delay
 * vs no filter. (0)disable, (1)enable */
void setTmr1NoiseFilter(char enable) {
	if(enable)
		TCCR1B |= (1 << ICNC1);
	else
		TCCR1B &= ~(1 << ICNC1);
}

//------------------------TIMER 2-------------------------

unsigned char *TMR2_OUT_PORT = &PORTD;
unsigned char *TMR2_OUT_DIR = &DDRD;
#define TMR2_OUT_COMP_MATCH_PIN (1 << PD7)

/* Insert the following code snippets into your own
 * functions as needed.*/

// TOSC1 Pin = PC6
// TOSC2 Pin = PC7

// Get status of output compare match
// TIFR & (1 << OCF2);
// Get status of timer overflow flag
// TIFR & (1 << TOV2);

// Clear OCF2 flag
// TIFR = (1 << OCF2);
// Clear TOV2 flag
// TIFR = (1 << TOV2);

// Check if timer2 Timer/Counter Register is busy updating
// ASSR & (1 << TCN2UB);
// Check if timer2 Output Compare Register is busy updating
// ASSR & (1 << OCR2UB);
// Check if timer2 Timer Control Register is busy updating
// ASSR & (1 << TCR2UB);

/* timer2 interrupt handlers
ISR(TIMER2_COMP_vect) {
	// handle interrupt
}

ISR(TIMER2_OVF_vect) {
	// handle interrupt
}
*/

/* Sets output dir and port for compare match out. */
void initTmr2OutPort() {
	*TMR2_OUT_DIR |= TMR2_OUT_COMP_MATCH_PIN;
	*TMR2_OUT_PORT &= ~TMR2_OUT_COMP_MATCH_PIN;
}

/* Determines functionality of timer2.  See AVR data sheet
 * for details on timer functions. Use enum TIMER02_MODES
 * and enum TIMER_COMPARE_MODES values for arguments. */
void setTmr2Mode(int timerMode, int compareOutMode) {
	TCCR2 &= ~((1 << COM20) | (1 << COM21) | (1 << WGM20) | (1 << WGM21));
	switch(timerMode) {
		case TMR02_NORMAL_MODE:
			// Normal Mode
			break;
		case TMR02_PHASE_CORRECT_PWM_MODE:
			TCCR0 |= (1 << WGM20);
			break;
		case TMR02_CTC_MODE:
			TCCR0 |= (1 << WGM21);
			break;
		case TMR02_FAST_PWM_MODE:
			TCCR0 |= (1 << WGM20) | (1 << WGM21);
			break;
		// default normal mode
	}
	
	switch(compareOutMode) {
		case TMR_COMPARE_NORMAL_MODE:
			// Normal Mode
			break;
		case TMR_COMPARE_TOGGLE_MODE:
			TCCR0 |= (1 << COM20);
			break;
		case TMR_COMPARE_CLEAR_MODE:
			TCCR0 |= (1 << COM21);
			break;
		case TMR_COMPARE_SET_MODE:
			TCCR0 |= (1 << COM20) | (1 << COM21);
			break;
		// default normal mode
	}
}

/* Modifies the timer/counter speed by dividing the system
 * clock speed. Use enum PRESCALERS values for the argument.
 * Timer starts as soon as prescaler is set. */
void setTmr2Prescaler(int prescaler) {
	TCCR2 &= ~((1 << CS20) | (1 << CS21) | (1 << CS22));
	switch(prescaler) {
		case TMR_PRESCALER_OFF:
			TCCR0 |= (1 << CS20);
			break;
		case TMR_PRESCALER_8TH:
			TCCR0 |= (1 << CS21);
			break;
		case TMR_PRESCALER_64TH:
			TCCR0 |= (1 << CS21) | (1 << CS20);
			break;
		case TMR_PRESCALER_256TH:
			TCCR0 |= (1 << CS22);
			break;
		case TMR_PRESCALER_1024TH:
			TCCR0 |= (1 << CS22) | (1 << CS20);
			break;
		case TMR_PRESCALER_EXT_FALL:
			TCCR0 |= (1 << CS22) | (1 << CS21);
			break;
		case TMR_PRESCALER_EXT_RISE:
			TCCR0 |= (1 << CS22) | (1 << CS21) | (1 << CS20);
			break;
		default:
			TCCR0 |= (1 << CS20); // Prescaler off
	}
}

/* Clears any prescaler setting for timer2 such that
 * the timer operates at the system clock speed. */
void resetTmr2Prescaler() {
	SFIOR |= (1 << PSR2);
}

/* Set the TOP value at which the timer1 output
 * compare flag will be set and the timer will reset. */
void setTmr2CompareVal(unsigned char time) {
	OCR2 = time;
}

/* Forces the output compare pin to act
 * as though an actual compare occurred but does not
 * affect the output compare flag or interrupt if enabled. */
void forceCompareTmr2() {
	TCCR2 |= (1 << FOC2);
}

/* Stops timer2 but does not reset it. */
void stopTmr2() {
	TCCR2 &= ~((1 << CS20) | (1 << CS21) | (1 << CS22));
}

/* Sets the time at which the timer will start counting.
 * Temporarily disables global interrupts. */
void setTmr2(unsigned char time) {
	unsigned char sreg;
	
	sreg = SREG;
	SREG &= 0x7F;
	TCNT2 = time;
	SREG = sreg;
}

/* Returns the current value in the timer2 register.
 * Temporarily disables global interrupts. */
unsigned char readTmr2() {
	unsigned char sreg, temp;
	
	sreg = SREG;
	SREG &= 0x7F;
	temp = TCNT2;
	SREG = sreg;
	return temp;
}

/* Enable or disable any of the available interrupts
 * for timer1.  Use the enum TIMER02_INTERRUPTS values
 * for the interrupt argument. (0)disable, (1)enable*/
void enableTmr2Interrupts(int interrupt, char enable) {
	switch(interrupt) {
		case TMR02_COMPARE_INTERRUPT:
			if(enable)
				TIMSK |= (1 << OCIE2);
			else
				TIMSK &= ~(1 << OCIE2);
			break;
		case TMR02_OVERFLOW_INTERRUPT:
			if(enable)
				TIMSK |= (1 << TOIE2);
			else
				TIMSK &= ~(1 << TOIE2);
			break;
		// default do nothing
	}
}

/* Enable or disable external clock input for timer2. 
 * (0)disable, (1)enable */
void enableExtClkTmr2(char enable) {
	if(enable)
		ASSR |= (1 << AS2);
	else
		ASSR &= ~(1 << AS2);
}

#endif