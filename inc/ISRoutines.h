/*
 * ISRoutines.h
 *
 *  Created on: 5 de mai de 2020
 *      Author: carol
 */

#ifndef ISROUTINES_H_
#define ISROUTINES_H_

#include "stm32f10x.h"

extern volatile int count;
extern "C" void TIM4_IRQHandler(void){
	if (TIM4->SR & TIM_SR_UIF) {
		TIM4->SR &= ~TIM_SR_UIF;
		count++;

	}
}



#endif /* ISROUTINES_H_ */
