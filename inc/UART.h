/* UART.h
 *
 * USART Interface
 *
 * 					TX		RX
 * USART1			PA9		PA10
 * USART1 (REMAP)	PB6		PB7
 * USART2			PA2		PA3
 * USART3			PB10    PB11
 */

#ifndef UART_H_
#define UART_H_

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "stdarg.h"
#include "stm32f10x.h"
#include "system_stm32f10x.h"

#define DEBUG_UART 	1

class UART {
public:
	USART_TypeDef * USARTx;
	uint32_t 		clockMHz;
	UART(USART_TypeDef * USARTx);

	void Initialize(uint32_t baudR);
	void printMsg(char *msg, ...);
};

#endif /* UART_H_ */
