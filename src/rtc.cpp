/*
 * rtc.cpp
 *
 *  Created on: 10 de jun de 2020
 *      Author: carol
 */

#include "rtc.h"

void configRTC(void){
	//1. Poll RTOFF, wait until its value goes to ‘1’
	while ( !( RTC->CRL & RTC_CRL_RTOFF));
	//2. Set the CNF bit to enter configuration mode
	RTC->CRL |= RTC_CRL_CNF;
	//3. Write to one or more RTC registers
	RTC->PRLH |= ( ((__RTC_PERIOD*__RTCCLK/1000-1) >> 16) & 0x00FF);
	RTC->PRLL |= ( (__RTC_PERIOD*__RTCCLK/1000-1) & 0xFFFF);
	RTC->CNTH |= ( (__RTC_CNT >> 16) & 0x00FF);
	RTC->CNTL |= (__RTC_CNT & 0xFFFF);

	//4. Clear the CNF bit to exit configuration mode
	RTC->CRL &= ~RTC_CRL_CNF;
	//5. Poll RTOFF, wait until its value goes to ‘1’ to check the end of the write operation.
	while ( !( RTC->CRL & RTC_CRL_RTOFF));

}

/*RTC*/
void SetUpRTC(void){
	//1. Enable the power and backup interface clocks by setting the PWREN and BKPEN bits in the RCC_APB1ENR register
	RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;

	//2. set the DBP bit the Power Control Register (PWR_CR) to enable access to the Backup registers and RTC.
	PWR->CR |= PWR_CR_DBP;

	//LSE as CLKSRC
	RCC->BDCR |= RCC_BDCR_LSEON;
	while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0);
//	RCC->CSR |= RCC_CSR_LSION;
//	while ((RCC->CSR & RCC_CSR_LSIRDY) == 0);

	RCC->BDCR |= (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_LSE) ;
	configRTC();

	PWR->CR &= ~PWR_CR_DBP;

/*	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI); //HSE/128, LSE or LSI clocks
	RCC_RTCCLKCmd(ENABLE);

	RTC_SetPrescaler(1);
	*/
	//RTC_Init();
}


RTC_values_t readRTC(){
	uint32_t CNTH, CNTL, CNT;
	RTC_values_t data;
	CNTH = (uint32_t) (RTC->CNTH);
	CNTL = (uint32_t) RTC->CNTL;

	CNT = ( CNTH << 15 |  CNTL);
	data.hours = CNT/3600;
	data.minutes = (CNT % 3600)/60;
	data.seconds = (CNT % 3600) % 60;

	return data;
}

