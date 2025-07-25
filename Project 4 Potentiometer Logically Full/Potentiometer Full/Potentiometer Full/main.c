/*
* Potentiometer Full.c
*
* Created: 18-07-2025 16:12:21
* Author : vijayant
*/

#include "main.h"

int main(void)
{
	USART0_Init(103); // 9600 Baud FOR DOCKLIGHT
	USART1_Init(8);	  // 115200 Baud FOR BLE
	USART2_Init(103); // 9600 Baud FOR PLOTTER
	USART3_Init(103); // 9600 Baud FOR MODBUS
	sei();			  // Enable global interrupts

	InitADC();
//	USART0_Print("\nRESTART\n");
	/* Replace with your application code */
	while (1)
	{
		application();
	}
}

