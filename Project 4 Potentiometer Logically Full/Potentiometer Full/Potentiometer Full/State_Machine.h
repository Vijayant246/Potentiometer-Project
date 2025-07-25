/*
* State_Machine.h
*
* Created: 21-07-2025 14:26:32
*  Author: vijayant
*/


#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include "main.h"

//For ADC = 3 Calculation
float adc_value;
float voltage_out, current;
float ref_resistance, pot_resistance, power, energy;
volatile uint8_t rx_buffer[8];
char dbg[512];
// Temporary buffer for float-to-string conversion
char val_buf[16];

char adc[32], volt[32], ref_R[48], pot_R[48], cur[32], Pow[32], enrgy[32];

typedef enum {
	ST_INIT,
	SEND_WAIT_CMD,
	STATE_PARSE_COMMAND,
	ST_READADC,
	ST_WAIT_REQUEST,
	SEND_2_ALL,
	ST_RESET,
}STATE_M;

void application(void);

#endif /* STATE_MACHINE_H_ */