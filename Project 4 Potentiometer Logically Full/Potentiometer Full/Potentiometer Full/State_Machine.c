/*
* State_Machine.c
*
* Created: 21-07-2025 14:26:43
*  Author: vijayant
*/

#include "State_Machine.h"  // Include the state machine header
#include "MODBUS.h"         // Include the MODBUS communication header

static STATE_M current_state = ST_INIT; // Initialize the current state to ST_INIT
//static send_flag_t send_flag = SEND_NONE; // Initialize the current state to SEND_NONE

#define SLAVE_ID 0x05      // Define the slave ID for MODBUS communication
#define voltage_ref 5.0    // Define the reference voltage for measurements

int valid = 0;              // Initialize a variable to track validity

// For Command Print on Docklight
char cmd[CMD_BUFFER_SIZE];
char cmdline[512];
char dbg1[512];
uint16_t byte;
uint32_t cmd_len = 0;

/* For the unknown resistor */
#define R_KNOWN 200.5      // Define the known resistor value in ohms
#define DELAY_SEC 0.5      // Define the delay between updates in seconds

void send_to_terminal(void) { // Send to Docklight
	_delay_ms(1000); // Introduces a 1-second delay
	USART0_Print("\nConnected to Docklight\r\n"); // Sends a connection message
	USART0_Print(adc); // Sends the ADC value
	USART0_Print(ref_R); // Sends the reference resistance
	USART0_Print(pot_R); // Sends the potentiometer resistance
	USART0_Print(volt); // Sends the voltage
	USART0_Print(cur); // Sends the current
	USART0_Print(Pow); // Sends the power
	USART0_Print(enrgy); // Sends the energy
}

void send_to_ble(void) { // Send to BLE
	_delay_ms(1000); // Introduces a 1-second delay
	USART1_Print("\nConnect to BM71 BLE \r\n"); // Sends a connection message for BLE
	USART1_Print(adc); // Sends the ADC value
	_delay_ms(100);
	USART1_Print(ref_R); // Sends the reference resistance
	_delay_ms(100);
	USART1_Print(pot_R); // Sends the potentiometer resistance
	_delay_ms(100);
	USART1_Print(volt); // Sends the voltage
	_delay_ms(100);
	USART1_Print(cur); // Sends the current
	_delay_ms(100);
	USART1_Print(Pow); // Sends the power
	_delay_ms(100);
	USART1_Print(enrgy); // Sends the energy
	_delay_ms(100);
}

void send_to_plotter(void) {// Send to plotter
	char dbg[512]; // Buffer to hold the formatted string
	
	//sprintf(dbg, "%f %f %f %f %f %f\n", ref_resistance, pot_resistance, voltage_out, current, power, energy); // Formats the data Better Serial Plotter
	
	sprintf(dbg, "$%f %f %f %f %f %f;", ref_resistance, pot_resistance, voltage_out, current, power, energy); // Formats the data for the Serial Port Plotter
	
	USART2_Print(dbg); // Sends the formatted string to the plotter
}

void send_to_modbus(void) { // Send to Modbus
	send_modbus_response(); // Sends a response via Modbus
}


void application(void)
{
	// 	static uint8_t prevState = 0xFF; // Initialize previous state to an invalid value
	//
	// 	// Check if the current state is different from the previous state
	// 	if (prevState != current_state) {
	// 		char hex[20]; // Buffer to hold the formatted string
	// 		prevState = current_state; // Update the previous state
	// 		sprintf(hex, " \n State = %d \n", current_state); // Format the output
	// 		USART0_Print(hex); // Send the output to USART
	// 	}
	
	switch (current_state)
	{
		case ST_INIT:
		{
			USART0_Print("SEND CMD LINE\n");
			USART0_Print("1. TERMINAL\n2. BLE\n3. PLOT\n4. MODBUS\n5. ALL");
			USART1_Print("SEND CMD LINE");
			USART1_Print("1. TERMINAL\n2. BLE\n3. PLOT\n4. MODBUS\n5. ALL");
			cmd_len = 0; // Reset command length
			current_state = SEND_WAIT_CMD; // Move to the next state
			break;
		}
		
		case SEND_WAIT_CMD:
		{
			if (USART0_RxByte(&byte) || USART1_RxByte(&byte)) {
				USART0_Transmit(byte); // Echo to USART0
				if (byte == '\r' || byte == '\n') {
					if (cmd_len > 0) {
						cmd[cmd_len] = '\0'; // Null-terminate the command
						current_state = STATE_PARSE_COMMAND; // Transition to parsing state
					}
					else {
						cmd_len = 0; // Reset buffer length if no command
					}
				}
				else if (cmd_len < CMD_BUFFER_SIZE - 1) {
					cmd[cmd_len++] = byte; // Append byte to command buffer
				}
			}
			break;
		}
		
		case STATE_PARSE_COMMAND:
		{
			int parsed = sscanf(cmd, "%511s", cmdline); // Parse command
			snprintf(dbg1, sizeof(dbg1), "P = %d CMD = \"%s\" \r\n", parsed, cmd);
			USART0_Print(dbg1); // Print debug information

			for (int i = 0; cmdline[i]; i++){
				cmdline[i] = tolower(cmdline[i]); // Convert command to lowercase
			}

			if (parsed == 1) {
				valid = 1; // Command is valid
				current_state = ST_READADC; // Transition to read ADC state
			}
			
			break;
		}

		
		case ST_READADC:
		{
			adc_value = ReadADC(3); // Read from A3 (ADC3)
			voltage_out = (adc_value * voltage_ref) / 1023.0; // Convert ADC value to voltage (in Volts)
			ref_resistance = (adc_value * 100) / 1023.0; // Calculate reference resistance (in kOhm)
			pot_resistance = R_KNOWN * (voltage_out / (voltage_ref - voltage_out)); // Calculate potential resistance (in Ohm)
			current = voltage_out / pot_resistance; // Calculate current (in Amperes)
			power = (voltage_out * current); // Calculate power (in Watts)
			energy += power * DELAY_SEC; // Accumulate energy (in Watt-seconds)
			
			// Format output strings using dtostrf for BLE-safe ASCII
			dtostrf(adc_value, 6, 3, val_buf);
			sprintf(adc, "ADC = %s\r\n", val_buf);

			dtostrf(voltage_out, 6, 3, val_buf);
			sprintf(volt, "Voltage = %s V\r\n", val_buf);

			dtostrf(ref_resistance, 6, 3, val_buf);
			sprintf(ref_R, "Resistance Reference = %s kOhm\r\n", val_buf);

			dtostrf(pot_resistance, 6, 3, val_buf);
			sprintf(pot_R, "Resistance Potential = %s Ohm\r\n", val_buf);

			dtostrf(current, 6, 6, val_buf);
			sprintf(cur, "Current = %s A\r\n", val_buf);

			dtostrf(power, 6, 6, val_buf);
			sprintf(Pow, "Power = %s W\r\n", val_buf);

			dtostrf(energy, 6, 6, val_buf);
			sprintf(enrgy, "Energy = %s Ws\r\n", val_buf);
			
			// Store calculated values in a data structure for further processing
			modbus_data.ref_resistance = (uint16_t)(ref_resistance);
			modbus_data.pot_resistance = (uint16_t)(pot_resistance);
			modbus_data.voltage_mV = (uint16_t)(voltage_out * 100);
			modbus_data.current_uA = (uint16_t)(current * 1000000);
			modbus_data.power_uW = (uint16_t)(power * 1000000);
			modbus_data.energy_uWs = (uint16_t)(energy * 1000);
			
			current_state = SEND_2_ALL; // Transition to the next state
			break;
		}

		
		case ST_WAIT_REQUEST:
		{
			// Check if a request is ready to be processed
			if (request_ready) {
				request_ready = 0; // Reset the request flag
				rx_index = 0; // Reset the receive index

				// Validate the request by checking the slave ID and function code
				if (rx_buffer[0] == SLAVE_ID && rx_buffer[1] == 0x10) {
					uint8_t byte_count = rx_buffer[6]; // Get the byte count from the request
					uint8_t expected_length = 7 + byte_count + 2; // Calculate expected length

					// Extract the received CRC from the buffer
					uint16_t crc_received = (rx_buffer[expected_length - 1] << 8) | rx_buffer[expected_length - 2];
					// Calculate the CRC for validation
					uint16_t crc_calc = modbus_crc16((uint8_t *)rx_buffer, expected_length - 2);

					// Compare received CRC with calculated CRC
					if (crc_received == crc_calc) {
						// If valid, process the data
						for (uint8_t i = 0; i < byte_count / 2; i++) {
							modbus_data.registers[i] = (rx_buffer[7 + i * 2] << 8) | rx_buffer[8 + i * 2];
						}
						current_state = SEND_2_ALL; // Transition to the next state
					}
				}
			}
			break;
		}
		
		case SEND_2_ALL:
		{
			if (strcmp(cmdline, "terminal") == 0)  // bitwise check
			{
				send_to_terminal();
			}

			else if (strcmp(cmdline, "ble") == 0)
			{
				send_to_ble();
			}

			else if (strcmp(cmdline,"plot") ==0)
			{
				send_to_plotter();
			}

			else if (strcmp(cmdline, "modbus") == 0)
			{
				send_to_modbus();
			}
			
			else if (strcmp(cmdline, "all") == 0)
			{
				int count = 0;
				while (count < 30) // or any number of cycles you want
				{
					// ?? Read and process new ADC data
					adc_value = ReadADC(3);
					voltage_out = (adc_value * voltage_ref) / 1023.0;
					ref_resistance = (adc_value * 100) / 1023.0;
					pot_resistance = R_KNOWN * (voltage_out / (voltage_ref - voltage_out));
					current = voltage_out / pot_resistance;
					power = voltage_out * current;
					energy += power * DELAY_SEC;

					// ?? Format updated values into strings
					snprintf(adc, sizeof(adc), "Adc = %f \r\n", adc_value);
					snprintf(volt, sizeof(volt), "Voltage = %f V\r\n", voltage_out);
					snprintf(ref_R, sizeof(ref_R), "Resistance Reference = %f kOhm\r\n", ref_resistance);
					snprintf(pot_R, sizeof(pot_R), "Resistance Potential = %f Ohm\r\n", pot_resistance);
					snprintf(cur, sizeof(cur), "Current = %f A\r\n", current);
					snprintf(Pow, sizeof(Pow), "Power = %f W\r\n", power);
					snprintf(enrgy, sizeof(enrgy), "Energy = %f Ws\r\n", energy);

					// ?? Update MODBUS structure
					modbus_data.ref_resistance = (uint16_t)(ref_resistance);
					modbus_data.pot_resistance = (uint16_t)(pot_resistance);
					modbus_data.voltage_mV = (uint16_t)(voltage_out * 100);
					modbus_data.current_uA = (uint16_t)(current * 1000000);
					modbus_data.power_uW = (uint16_t)(power * 1000000);
					modbus_data.energy_uWs = (uint16_t)(energy * 1000);

					// ?? Send updated values
					send_to_terminal();
					send_to_modbus();
					send_to_ble();
					send_to_plotter();

					_delay_ms(100);  // Wait before next round
					count++;
				}
				current_state = ST_RESET; // Reset at the end of loop
				break;
			}
			
			else
			{
				valid = 0;
				USART0_Print("ERR CMD\r\n");
				USART1_Print("ERR CMD\r\n");
				current_state = ST_RESET; //move to next state
				break;
			}
		}
		
		case ST_RESET:
		{
			_delay_ms(1000);
			cmd_len = 0;
			current_state = ST_INIT; //move to next state
			break;
		}
	}
}