/*
 * USART.h
 *
 * Created: 18-07-2025 16:13:40
 *  Author: vijayant
 */ 


#ifndef USART_H_
#define USART_H_

#include "main.h"


//=== Function Prototypes ===

//=== USART0: PC (Docklight) <-> Arduino ===
//UART 0 Function
void USART0_Init(uint16_t ubrr);
void USART0_Transmit(unsigned char data);
void USART0_Print(const char *str);
uint8_t USART0_RxByte(uint16_t *buff);

//=== USART1: Arduino <-> BLE ===
//UART 1 Function
void USART1_Init(uint16_t ubrr);
void USART1_Transmit(unsigned char data);
void USART1_Print(const char *str);
uint8_t USART1_RxByte(uint16_t *buff);

//=== USART2: Arduino to Plotter ===
//UART 2 Function
void USART2_Init(uint16_t ubrr);
void USART2_Transmit(unsigned char data);
void USART2_Print(const char *str);
uint8_t USART2_RxByte(uint16_t *buff);

//=== USART3: Arduino to MODBUS ===
//UART 3 Function
void USART3_Init(uint16_t ubrr);
void USART3_Transmit(unsigned char data);
void USART3_Print(const char *str);
uint8_t USART3_RxByte(uint16_t *buff); 


#endif /* USART_H_ */