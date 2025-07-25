/*
* MODBUS.c
*
* Created: 21-07-2025 14:27:00
*  Author: vijayant
*/

#include "MODBUS.h"

volatile uint8_t rx_index = 0;
volatile uint8_t request_ready = 0;
volatile uint8_t rx_buffer[8];

// Define the global modbus_data
ModbusPacket modbus_data;

uint16_t modbus_crc16(uint8_t *buffer, uint8_t length) {
	uint16_t crc = 0xFFFF;
	for (uint8_t i = 0; i < length; i++) {
		crc ^= buffer[i];
		for (uint8_t j = 0; j < 8; j++) {
			if (crc & 0x0001)
			crc = (crc >> 1) ^ 0xA001;
			else
			crc >>= 1;
		}
	}
	return crc;
}

void send_modbus_response() {
	uint8_t tx_buf[21];
	uint8_t i = 0;

	tx_buf[i++] = SLAVE_ID;
	tx_buf[i++] = 0x10; // Function code: Write Multiple Registers
	tx_buf[i++] = 0x00; // Start Address High
	tx_buf[i++] = 0x01; // Start Address Low
	tx_buf[i++] = 0x00; // Quantity High
	tx_buf[i++] = 0x06; // 6 registers
	tx_buf[i++] = 12;   // Byte count: 6 * 2 = 12

	tx_buf[i++] = modbus_data.ref_resistance >> 8;
	tx_buf[i++] = modbus_data.ref_resistance & 0xFF;

	tx_buf[i++] = modbus_data.pot_resistance >> 8;
	tx_buf[i++] = modbus_data.pot_resistance & 0xFF;

	tx_buf[i++] = modbus_data.voltage_mV >> 8;
	tx_buf[i++] = modbus_data.voltage_mV & 0xFF;

	tx_buf[i++] = modbus_data.current_uA >> 8;
	tx_buf[i++] = modbus_data.current_uA & 0xFF;

	tx_buf[i++] = modbus_data.power_uW >> 8;
	tx_buf[i++] = modbus_data.power_uW & 0xFF;

	tx_buf[i++] = modbus_data.energy_uWs >> 8;
	tx_buf[i++] = modbus_data.energy_uWs & 0xFF;

	uint16_t crc = modbus_crc16(tx_buf, i);
	tx_buf[i++] = crc & 0xFF;
	tx_buf[i++] = crc >> 8;

	for (int j = 0; j < i; j++) {
		USART3_Transmit(tx_buf[j]);
	}
}