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
extern Timer Timer2;
extern volatile long unsigned int ulHighFrequencyTimerTicks;

extern "C" void TIM2_IRQHandler(void) {
    if((TIM2->SR & TIM_SR_UIF) != 0)
    {// If update flag is set
    	Timer2.SysTime++;
    	ulHighFrequencyTimerTicks++;
    }

    TIM2->SR &= ~TIM_SR_UIF; //clear interrupt flag

}



extern "C" void TIM4_IRQHandler(void){
	if (TIM4->SR & TIM_SR_UIF) {
		TIM4->SR &= ~TIM_SR_UIF;
		count++;

	}
}



#endif /* ISROUTINES_H_ */
