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

#include <avr/adc.h>

void setADCprescaler(int prescaler) {
	ADCSRA &= ~((1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2));
	switch(prescaler) {
		case ADC_PRESCALER_HALF:
			// 1/2 speed
			break;
		case ADC_PRESCALER_QTR:
			ADCSRA |= (1 << ADPS1);
			break;
		case ADC_PRESCALER_8TH:
			ADCSRA |= (1 << ADPS0) | (1 << ADPS1);
			break;
		case ADC_PRESCALER_16TH:
			ADCSRA |= (1 << ADPS2);
			break;
		case ADC_PRESCALER_32ND:
			ADCSRA |= (1 << ADPS0) | (1 << ADPS2);
			break;
		case ADC_PRESCALER_64TH:
			ADCSRA |= (1 << ADPS1) | (1 << ADPS2);
			break;
		case ADC_PRESCALER_128TH:
			ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
			break;
		// default: ADC_PRESCALER_HALF
	}
}

void setADCrefVltg(int refrnc) {
	ADMUX &= ~((1 << REFS0) | (1 << REFS1));
	switch(refrnc) {
		case AREF_VOLTAGE:
			// Use voltage at AREF pin for reference
			break;
		case AVCC_VOLTAGE:
			ADMUX |= (1 << REFS0);
			break;
		case INTERNAL_2_56_VOLTAGE:
			ADMUX |= (1 << REFS0) | (1 << REFS1);
			break;
		//default: AREF_VOLTAGE
	}
}

void setADCsingleEndChannel(unsigned char channel) {
	ADMUX &= 0xE0;
	if(channel > 9)
		channel = 0;
	if(channel < 8)
		ADMUX |= channel;
	else {
		switch(channel) {
			case 8:
				ADMUX |= 0x1E; // MUX[4:1]
				break;
			case 9:
				ADMUX |= 0x1F; // MUX[4:0]
		}
	}
}

void setADCdiffChannels(int gain, unsigned char posInChannel, unsigned char negInChannel) {
	ADMUX &= 0xC0;
	switch(gain) {
		case x1_ADC_GAIN:
			ADMUX |= 0x10; // MUX[4]
			switch(negInChannel) {
				case 1:
					if(posInChannel > 7)
						posInChannel = 0;
					ADMUX |= posInChannel;
					break;
				case 2:
					ADMUX |= 0x08; // MUX[3]
					if(posInChannel > 5)
						posInChannel = 0;
					ADMUX |= posInChannel;
					break;
				// default: ADC0(+) ADC1(-)
			}
			break;
		case x10_ADC_GAIN:
			ADMUX |= 0x08;
			switch(negInChannel) {
				case 0:
					if(posInChannel > 1)
						posInChannel = 0;
					ADMUX |= posInChannel;
					break;
				case 2:
					ADMUX |= 0x04 | (posInChannel % 2);
					break;
				// default: 
			}
			break;
		case x200_ADC_GAIN:
			ADMUX |= 0x08;
			switch(negInChannel) {
				case 0:
					if(posInChannel > 1)
						posInChannel = 0;
					ADMUX |= 0x02 | posInChannel;
					break;
				case 2:
					ADMUX |= 0x06 | (posInChannel % 2);
					break;
				// default: 
			}
			break;
		default:
			ADMUX  |= 0x10; // x1_ADC_GAIN
	}
}

void setADCautoTriggerSource(char onOff, int triggerSource) {
	if(onOff)
		ADCSRA |= (1 << ADATE);
	else
		ADCSRA &= ~(1 << ADATE);

	SFIOR &= ~((1 << ADTS0) | (1 << ADTS1) | (1 << ADTS2));
	switch(triggerSource) {
		case FREE_RUN:
			// Free run mode
			break;
		case ANALOG_COMPARATOR:
			SFIOR |= (1 << ADTS0);
			break;
		case EXT_INTERRUPT_REQUEST_0:
			SFIOR |= (1 << ADTS1);
			break;
		case TMR0_CMPR_MATCH:
			SFIOR |= (1 << ADTS0) | (1 << ADTS1);
			break;
		case TMR0_OVERFLOW:
			SFIOR |= (1 << ADTS2);
			break;
		case TMR1_CMPR_MATCH_B:
			SFIOR |= (1 << ADTS0) | (1 << ADTS2);
			break;
		case TMR1_OVERFLOW:
			SFIOR |= (1 << ADTS1) | (1 << ADTS2);
			break;
		case TMR1_CAPTURE_EVENT:
			SFIOR |= (1 << ADTS0) | (1 << ADTS1) | (1 << ADTS2);
			break;
		// default: Free run mode
	}
}

void enableADCinterrupt(char enable) {
	if(enable)
		ADCSRA |= (1 << ADIE);
	else
		ADCSRA &= ~(1 << ADIE);
}

void enableADC(char enable) {
	if(enable)
		ADCSRA |= (1 << ADEN);
	else
		ADCSRA &= ~(1 << ADEN);
}

short getADCreading(int resolution) {
	if(resolution == EIGHT_BIT_RES)
		ADMUX |= (1 << ADLAR);
	else
		ADMUX &= ~(1 << ADLAR);

	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC))
		continue;
	
	if(resolution == EIGHT_BIT_RES)
		return ADCH;
	
	short temp = ADCL;
	temp |= (ADCH << 8);
	return temp;
}

void startADCreading(int resolution) {
	if(resolution == EIGHT_BIT_RES)
		ADMUX |= (1 << ADLAR);
	else
		ADMUX &= ~(1 << ADLAR);
		
	ADCSRA |= (1 << ADSC);
}

short retrieveADCreading(int resolution) {
	if(ADCSRA & (1 << ADSC))
		return 0x7FFF;
	return (resolution == EIGHT_BIT_RES) ? ADCH : (ADCL | (ADCH << 8));	
}	
