/*
 * Subsystems.h
 *
 *  Created on: 16 de abr de 2020
 *      Author: carol
 */

#ifndef SUBSYSTEMS_H_
#define SUBSYSTEMS_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

class Subsystems {
public:
	GPIO_TypeDef* 	GPIOx;
	uint32_t 		GPIO_RCCx;
	uint16_t 		GPIO_Pin;
	uint8_t 		status;

	Subsystems(GPIO_TypeDef* GPIOx, uint32_t GPIO_RCCx, uint16_t GPIO_Pin);
	virtual ~Subsystems();

	void configurePin();
	uint8_t cmdSystem(FunctionalState Status);

};

#endif /* SUBSYSTEMS_H_ */
