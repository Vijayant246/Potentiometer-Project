/*
* main.h
*
* Created: 18-07-2025 16:52:36
*  Author: vijayant
*/


#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 16000000UL
#define RX_BUFFER_SIZE 1024
#define CMD_BUFFER_SIZE 1024

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "USART.h"
#include "ADC.h"
#include "MODBUS.h"
#include "State_Machine.h"

#endif /* MAIN_H_ */