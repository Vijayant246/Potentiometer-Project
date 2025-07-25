/*
 * ADC.c
 *
 * Created: 21-07-2025 14:35:00
 *  Author: vijayant
 */ 

#include "ADC.h"

void InitADC(void)
{
	// Reference voltage = AVcc, left adjust result = 0 and also Shifting of the number

	// Register Description

	/*
	ADMUX ? ADC Multiplexer Selection Register
	Bit	  7		6	  5		4	 3	 2	  1		0
	(0x7C) REFS1 REFS0 ADLAR MUX4 MUX3 MUX2 MUX1 MUX0
	*/

	ADMUX = (1 << REFS0);
	// ADMUX &= ~(1<<REFS1);

	//  ADCSRA ? ADC Control and Status Register A

	// 	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC Enable, Prescaler = 128 (16MHz/128 = 125kHz)
	// 																	   /* Because the ADC frequency generally b/w 50 kHz to 200 kHz*/
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Enable ADC, prescaler = 64

}

uint16_t ReadADC(uint8_t ch)
{
	// Select ADC Channel ch must be 0-15
	ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);

	// Start Single conversion
	ADCSRA |= (1 << ADSC);

	// Wait for conversion to complete
	while (!(ADCSRA & (1 << ADIF)));

	// Clear ADIF by writing one to it
	// Note you may be wondering why we have write one to clear it
	// This is standard way of clearing bits in io as said in datasheets.
	// The code writes '1' but it result in setting bit to '0' !!!

	ADCSRA |= (1 << ADIF);

	return (ADC);
}