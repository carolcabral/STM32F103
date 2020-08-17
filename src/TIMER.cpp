/*
 * TIMER.cpp
 *
 *  Created on: 5 de mai de 2020
 *      Author: carol
 */

#include "TIMER.h"

volatile int count;

void delay (int us){
	TIM4->CR1 |= TIM_CR1_CEN;
	count = 0;
	while (count < us);
	TIM4->CR1 &= ~TIM_CR1_CEN;
}


TIMER::TIMER(TIM_TypeDef* TIMx) {
	//TIM1 and 8 APB2 / TIM 2-7 APB1
	this->TIMx = TIMx;

	if ( (TIMx == TIM1) || (TIMx == TIM8)){
		if (TIMx == TIM1) this->TIM_RCCx = RCC_APB2Periph_TIM1;
		else this->TIM_RCCx = RCC_APB2Periph_TIM8;
		RCC_APB2PeriphClockCmd(TIM_RCCx, ENABLE);//RCC_APB1Periph_TIMx
	} else {
		if (TIMx == TIM2) this->TIM_RCCx = RCC_APB1Periph_TIM2;
		if (TIMx == TIM3) this->TIM_RCCx = RCC_APB1Periph_TIM3;
		if (TIMx == TIM4) this->TIM_RCCx = RCC_APB1Periph_TIM4;
		if (TIMx == TIM5) this->TIM_RCCx = RCC_APB1Periph_TIM5;
		if (TIMx == TIM6) this->TIM_RCCx = RCC_APB1Periph_TIM6;
		if (TIMx == TIM7) this->TIM_RCCx = RCC_APB1Periph_TIM7;
		RCC_APB1PeriphClockCmd(TIM_RCCx, ENABLE);//RCC_APB1Periph_TIMx

	}
}

void TIMER::configDelay(void){
	/* DELAY*/
	//	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
	//	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	TIM4->PSC = 0;
	TIM4->ARR = 71;
	TIM4->CR1 |= TIM_CR1_URS; //Only counter overflow generates interrupt
	TIM4->DIER |= TIM_DIER_UIE; //Enable interrupts
	TIM4->EGR |= TIM_EGR_UG; //Update generation

	NVIC_EnableIRQ(TIM4_IRQn);


}
void TIMER::Initialize(void){
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


	/* PWM
	 	//Configure Pin

		GPIOB->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1;
		GPIOB->CRH &= (GPIO_CRH_CNF9_0);

		TIM4->CCER |= TIM_CCER_CC4E; //Enable Capture/Compare output enable
		TIM4->CR1 |= TIM_CR1_ARPE;
		TIM4->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE; //Config as PWM mode 1

		//PWM Frequency = Freq periph / [(PSC + 1) x ARR]
		//72MHZ / 1KHz = 72000
		TIM4->PSC = 71;
		TIM4->ARR = 1000;
		TIM4->CCR4 = 250; //Duty cycle

		TIM4->EGR |= TIM_EGR_UG;
		TIM4->CR1 |= TIM_CR1_CEN;
	 */

}

