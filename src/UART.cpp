/*
 * UART.cpp
 *
 *  Created on: 1 de mai de 2020
 *      Author: carol
 */

#include "UART.h"

UART::UART(USART_TypeDef * USARTx) {
	this->USARTx = USARTx;
	this->clockMHz = 72;

	// TODO Auto-generated constructor stub

}

/*USART1: TX - PA9  RX - PA10 / (REMAP) TX - PB6  RX - PB7*/
void UART::Initialize(uint32_t baudR){

	if (this->USARTx == USART1) {
		//Enable RCC
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_USART1EN;

		//Configure GPIO mode (AF_PP)
		GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1;
		GPIOA->CRH &= ~(GPIO_CRH_CNF9_0);

		GPIOA->CRH |= GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10_0 | GPIO_CRH_MODE10_1;
		GPIOA->CRH &= ~(GPIO_CRH_CNF10_0);


		/*REMAP:
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_USART1EN;
		GPIOB->CRL |= GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1;
		GPIOB->CRL &= ~(GPIO_CRL_CNF6_0);

		GPIOB->CRL |= GPIO_CRL_CNF7_1 | GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1;
		GPIOB->CRL &= ~(GPIO_CRL_CNF7_0);*/

		this->clockMHz = SystemCoreClock;

	} else if (this->USARTx == USART2) {
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

		GPIOA->CRL |= GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2_0 | GPIO_CRL_MODE2_1;
		GPIOA->CRL &= ~(GPIO_CRL_CNF2_0);

		GPIOA->CRL |= GPIO_CRL_CNF3_1 | GPIO_CRL_MODE3_0 | GPIO_CRL_MODE3_1;
		GPIOA->CRL &= ~(GPIO_CRL_CNF3_0);

		this->clockMHz = SystemCoreClock / 2;

	} else if (this->USARTx == USART3) {
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

		GPIOB->CRH |= GPIO_CRH_CNF10_1 | GPIO_CRH_MODE10_0 | GPIO_CRH_MODE10_1;
		GPIOB->CRH &= ~(GPIO_CRH_CNF10_0);

		GPIOB->CRH |= GPIO_CRH_CNF11_1 | GPIO_CRH_MODE11_0 | GPIO_CRH_MODE11_1;
		GPIOB->CRH &= ~(GPIO_CRH_CNF11_0);

		this->clockMHz = SystemCoreClock / 2;

	}


	/*Clock peripheral [72MHz] / (baudR [9600] x 16)
	  72 / 0.153600 = 468.75
	  BRR = 0x1d4(486)C(0.75*16)*/

	volatile uint32_t mantissa = (	this->clockMHz  / (baudR * 16) );
	volatile uint32_t fraction = ( this->clockMHz % (baudR * 16) );
	volatile uint32_t baudrate = (mantissa << 4) | (uint32_t) (((float) fraction) / (((float) baudR) * 16) * 16);
	this->USARTx->BRR = baudrate;
	this->USARTx->CR1 |= USART_CR1_TE;
	this->USARTx->CR1 |= USART_CR1_UE;


}


void UART::printMsg(char *msg, ...){
	char buff[80];

#ifdef DEBUG_UART
	va_list args;
	va_start(args, msg);
	vsprintf(buff,msg,args);

	for (int i=0; i < strlen(buff); i++){
		this->USARTx->DR = buff[i];
		while ( !(this->USARTx->SR & USART_SR_TXE));
	}
#endif
}
