/*
* MODBUS.h
*
* Created: 21-07-2025 14:27:28
*  Author: vijayant
*/


#ifndef MODBUS_H_
#define MODBUS_H_

#include "main.h"

#define SLAVE_ID 0x05


// Define ModbusPacket struct
typedef struct {
	uint16_t ref_resistance;   // Reference Resistance
	uint16_t pot_resistance;   // Potentiometer Resistance
	uint16_t voltage_mV;       // Voltage × 100
	uint16_t current_uA;       // Current × 1,000,000
	uint16_t power_uW;         // Power × 1,000,000
	uint16_t energy_uWs;       // Energy × 1,000
	uint16_t registers[6];  // Example: array of 6 registers
} ModbusPacket;

extern volatile uint8_t rx_index;
extern volatile uint8_t request_ready;
extern volatile uint8_t rx_buffer[8];

uint16_t modbus_crc16(uint8_t *buffer, uint8_t length);
void send_modbus_response(void);
extern ModbusPacket modbus_data;
#endif /* MODBUS_H_ */