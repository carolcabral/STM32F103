/*
 * gpio.cpp
 *
 *  Created on: 2 de ago de 2020
 *      Author: carol
 */

#include "gpio.h"

void portMode(GPIO_TypeDef *PORT, uint16_t PIN, uint32_t MODE){

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if (PORT == GPIOA) 		__HAL_RCC_GPIOA_CLK_ENABLE();
	else if (PORT == GPIOB) 	__HAL_RCC_GPIOB_CLK_ENABLE();
	else if (PORT == GPIOC) 	__HAL_RCC_GPIOC_CLK_ENABLE();
	else if (PORT == GPIOD)	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_SET);

	/*Configure GPIO pin : LED_PIN_Pin */
	GPIO_InitStruct.Pin = PIN;
	GPIO_InitStruct.Mode = MODE;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PORT, &GPIO_InitStruct);
}
