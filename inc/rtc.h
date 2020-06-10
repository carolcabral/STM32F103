/*
 * rtc.h
 *
 *  Created on: 10 de jun de 2020
 *      Author: carol
 */

#ifndef RTC_H_
#define RTC_H_

#include "stm32f10x.h"

#define __RTC_PERIOD  	0x000003E8

#define __RTCCLK		32768

#define __RTC_TIME_H	24 //0X0C
#define __RTC_TIME_M	0x00
#define __RTC_TIME_S	0X00

#define __RTC_CNT_TICKS	( (__RTC_TIME_H*3600UL) + (__RTC_TIME_M * 60UL) + (__RTC_TIME_S) )
#define __RTC_CNT	( __RTC_CNT_TICKS*1000UL/__RTC_PERIOD)

typedef struct RTC_values{
	float hours;
	float minutes;
	float seconds;

}RTC_values_t;


void configRTC(void);
void SetUpRTC(void);
RTC_values_t readRTC();



#endif /* RTC_H_ */
