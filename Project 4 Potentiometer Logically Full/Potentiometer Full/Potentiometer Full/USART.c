/*
 * USART.c
 *
 * Created: 18-07-2025 16:13:04
 *  Author: vijayant
 */ 

#include "USART.h"
#include "MODBUS.h"

//=== Global Buffers and Pointers ===
volatile char rx0Buff[RX_BUFFER_SIZE];
volatile char rx1Buff[RX_BUFFER_SIZE];
volatile char rx2Buff[RX_BUFFER_SIZE];
volatile char rx3Buff[RX_BUFFER_SIZE];

volatile uint16_t rx0ToDo = 0, rx0Done = 0;
volatile uint16_t rx1ToDo = 0, rx1Done = 0;
volatile uint16_t rx2ToDo = 0, rx2Done = 0;
volatile uint16_t rx3ToDo = 0, rx3Done = 0;

//=== USART0: PC (Docklight) <-> Arduino ===
void USART0_Init(uint16_t ubrr)
{
	UBRR0H = (ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
}

void USART0_Transmit(unsigned char data)
{
	while (!(UCSR0A & (1 << UDRE0)))
	;
	UDR0 = data;
}

void USART0_Print(const char *str)
{
	while (*str)
	USART0_Transmit(*str++);
}

//=== ISR: Docklight ? Arduino ===
ISR(USART0_RX_vect)
{
	char c = UDR0;
	rx0Buff[rx0ToDo++] = c;
	if (rx0ToDo >= RX_BUFFER_SIZE)
	rx0ToDo = 0;
}

//=== Buffered Read Functions ===
uint8_t USART0_RxByte(uint16_t *buff)
{
	if (rx0ToDo != rx0Done)
	{
		*buff = rx0Buff[rx0Done++];
		if (rx0Done >= RX_BUFFER_SIZE)
		rx0Done = 0;
		return 1;
	}
	return 0;
}

//=== USART1: Arduino <-> BLE ===
void USART1_Init(uint16_t ubrr)
{
	UBRR1H = (ubrr >> 8);
	UBRR1L = (uint8_t)ubrr;
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);
}

void USART1_Transmit(unsigned char data)
{
	while (!(UCSR1A & (1 << UDRE1)))
	;
	UDR1 = data;
}

void USART1_Print(const char *str)
{
	while (*str)
	USART1_Transmit(*str++);
}

//=== Buffered Read Functions ===
uint8_t USART1_RxByte(uint16_t *buff)
{
	if (rx1ToDo != rx1Done)
	{
		*buff = rx1Buff[rx1Done++];
		if (rx1Done >= RX_BUFFER_SIZE)
		rx1Done = 0;
		return 1;
	}
	return 0;
}

//=== ISR: BLE ? Arduino ===
ISR(USART1_RX_vect)
{
	char c = UDR1;
	rx1Buff[rx1ToDo++] = c;
	if (rx1ToDo >= RX_BUFFER_SIZE)
	rx1ToDo = 0;
}

//=== USART2: Arduino to Plotter ===
void USART2_Init(uint16_t ubrr)
{
	UBRR2H = (ubrr >> 8);
	UBRR2L = (uint8_t)ubrr;
	UCSR2C = (1 << UCSZ21) | (1 << UCSZ20);
	UCSR2B = (1 << RXEN2) | (1 << TXEN2) | (1 << RXCIE2);
}

void USART2_Transmit(unsigned char data)
{
	while (!(UCSR2A & (1 << UDRE2)))
	;
	UDR2 = data;
}

void USART2_Print(const char *str)
{
	while (*str)
	USART2_Transmit(*str++);
}

//=== Buffered Read Functions ===
uint8_t USART2_RxByte(uint16_t *buff)
{
	if (rx2ToDo != rx2Done)
	{
		*buff = rx2Buff[rx2Done++];
		if (rx2Done >= RX_BUFFER_SIZE)
		rx2Done = 0;
		return 1;
	}
	return 0;
}

//=== ISR: Plotter ? Arduino ===
ISR(USART2_RX_vect)
{
	char c = UDR2;
	rx2Buff[rx2ToDo++] = c;
	if (rx2ToDo >= RX_BUFFER_SIZE)
	rx2ToDo = 0;
}

//=== USART3: Arduino to MODBUS ===
void USART3_Init(uint16_t ubrr)
{
	UBRR3H = (ubrr >> 8);
	UBRR3L = (uint8_t)ubrr;
	UCSR3C = (1 << UCSZ31) | (1 << UCSZ30);
	UCSR3B = (1 << RXEN3) | (1 << TXEN3) | (1 << RXCIE3);
}

void USART3_Transmit(unsigned char data)
{
	while (!(UCSR3A & (1 << UDRE3)))
	;
	UDR3 = data;
}

void USART3_Print(const char *str)
{
	while (*str)
	USART3_Transmit(*str++);
}

//=== Buffered Read Functions ===
uint8_t USART3_RxByte(uint16_t *buff)
{
	if (rx3ToDo != rx3Done)
	{
		*buff = rx3Buff[rx3Done++];
		if (rx3Done >= RX_BUFFER_SIZE)
		rx3Done = 0;
		return 1;
	}
	return 0;
}

//=== ISR: MODBUS ? Arduino ===
ISR(USART3_RX_vect)
{
	uint8_t data = UDR3;
	if (rx_index < sizeof(rx_buffer)) {
		rx_buffer[rx_index++] = data;
		if (rx_index >= 8) {
			request_ready = 1;
		}
	}
}