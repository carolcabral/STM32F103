/*
 * led.cpp
 *
 *  Created on: 5 de mai de 2020
 *      Author: carol
 */


#include "stm32f10x.h"

void ledInit(void) {
	/*
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

	GPIOC->CRH |= GPIO_CRH_CNF13_0 | GPIO_CRH_CNF13_1;
	GPIOC->CRH &= ~ (GPIO_CRH_MODE13_0 | GPIO_CRH_MODE13_1);

	GPIOC->BSRR |= (1 << 13); //GPIO_BSRR_BS13;
	 */
	GPIO_InitTypeDef GPIO_InitStruct;
	// Configure PC13, PC14, PC15 as push-pull output
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void toggleLed(void){
		GPIOC->ODR ^= GPIO_Pin_13;
		//delay(1000);
}

