/*
 * rtc.cpp
 *
 *  Created on: 7 de ago de 2020
 *      Author: carol
 */

#include "rtc.h"

extern RTC_HandleTypeDef hrtc;

void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds){

	RTC_TimeTypeDef sTime = {0};

	sTime.Hours = hours;
	sTime.Minutes = minutes;
	sTime.Seconds = seconds;

	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
}
void setDate (uint8_t weekday, uint8_t date, uint8_t month, uint8_t year){
	RTC_DateTypeDef DateToUpdate = {0};

	DateToUpdate.WeekDay = weekday;
	DateToUpdate.Month = month;
	DateToUpdate.Date = date;
	DateToUpdate.Year = year;

	if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32f2);
}

RTC_TimeTypeDef getTime(void){
	RTC_TimeTypeDef gTime;

	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);

	return gTime;
	//sprintf((char*)time, "%02d:%02d:02d", gTime.Hours,gTime.Minutes, gTime.Seconds);
}

void RTCInit(){
	__HAL_RCC_RTC_ENABLE();

	hrtc.Instance = RTC;
	hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
	hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
	if (HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		Error_Handler();
	}
}
