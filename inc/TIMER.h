/**
  ******************************************************************************
  * @file    TIMER.h
  * @author  Ana Carolina Cabral
  * @date    05-May-2020
  * @brief   This file contains the implementation of a delay in microseconds (us) 
  *			 using hardware timers
  *
  * @todo	Add PWM interfacee
  			Add other timers (currently only supports TIM4)
  ******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

#include "stm32f10x.h"

void delay (int us);


class TIMER {
public:
	TIM_TypeDef* TIMx;
	uint32_t TIM_RCCx;
//	uint16_t TIMx_IRQn;



	TIMER(TIM_TypeDef* TIMx);
	void Initialize();
	void configDelay(void);

};



#endif /* TIMER_H_ */
