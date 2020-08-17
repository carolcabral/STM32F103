/*
 * clock.c
 *
 *  Created on: 10 de jun de 2020
 *      Author: carol
 */
#include "clock.h"

/*CLOCK*/
void SetClock72(void){
	//Chosing HSE clock source
	RCC->CR |= RCC_CR_HSEON;

	//Change to avoid deadlock
	while( !(RCC->CR & RCC_CR_HSERDY));

	FLASH->ACR |= FLASH_ACR_PRFTBE;
	FLASH->ACR &= ~(FLASH_ACR_LATENCY); //Clear
	/*							LATENCY
	 * 0 <  SYSCLK <= 24MHz			0
	 * 24MHz < SYSCLK <= 48MHz 		1
	 * 48MHz < SYSCLK <= 72MHz		2
	*/
	FLASH->ACR |= (uint32_t) 0x02;

	//COnfigure RCC
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL );

	RCC->CFGR &= ~(RCC_CFGR_PLLXTPRE); //HSE/2 if set
	RCC->CFGR |= RCC_CFGR_PLLSRC; //Clk from PREDIV1 if set / HSI_CLK/2 if not
	RCC->CFGR |= RCC_CFGR_PLLMULL9; //PLL multiplication factor
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

	RCC->CR |= RCC_CR_PLLON;

	//Change to avoid deadlock
	while( !(RCC->CR & RCC_CR_PLLRDY));

	RCC->CFGR &= ~(RCC_CFGR_SW);
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	//Change to avoid deadlock
	while( !(RCC->CFGR & RCC_CFGR_SWS_PLL));

	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);

}


