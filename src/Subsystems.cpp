/*
 * Subsystems.cpp
 *
 *  Created on: 16 de abr de 2020
 *      Author: carol
 */

#include "Subsystems.h"

Subsystems::Subsystems(GPIO_TypeDef* GPIOx, uint32_t GPIO_RCCx, uint16_t GPIO_Pin) {
	// TODO Auto-generated constructor stub
	this->GPIOx = GPIOx;
	this->GPIO_RCCx = GPIO_RCCx;
	this->GPIO_Pin = GPIO_Pin;
	this->status = ENABLE;
}

Subsystems::~Subsystems() {
	// TODO Auto-generated destructor stub

}

void Subsystems::configurePin(){
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(GPIO_RCCx, ENABLE);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOx, &GPIO_InitStruct);

	GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);
}

uint8_t Subsystems::cmdSystem(FunctionalState Status){
	if (Status == ENABLE) {
		GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_RESET);
	}
	else {
		GPIO_WriteBit(GPIOx, GPIO_Pin, Bit_SET);
	}
	return !GPIO_ReadOutputDataBit(GPIOx, GPIO_Pin);
}
