/*
 * Configure ADC1 (GPIOA) with multiple channels using DMA
 *
 *
 *
 * [BLUEPILL]
 *	2 ADC - 10 Channels
 * PA0 		ADC IN0
 * PA1 		ADC IN1
 * PA2		ADC IN2
 * PA3		ADC IN3
 * PA4		ADC IN4
 * PA5		ADC IN5
 * PA6		ADC IN6
 * PA7		ADC IN7
 *
 *
 *	[NOT WORKING:]
 * PB0 		ADC IN8
 * PB1		ADC IN9
 */

#ifndef ADC_H_
#define ADC_H_

#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

#define __NCHANNELS 	16

//ADC1_DR_ADDRESS ((uint32_t)0x4001204C)
class ADC {
public:
	static uint16_t 		ADC_Values[__NCHANNELS];
	static uint8_t 			ADC_CH[__NCHANNELS];
	//private:
	static uint8_t 			nchannel;
	static uint16_t 		ADC_PIN;
	static ADC_TypeDef* 	ADCx;
	static uint32_t 		ADC_RCC;
	static uint32_t 		ADC_GPIO_RCC;
	static GPIO_TypeDef*  	ADC_GPIO;


public:
	ADC(ADC_TypeDef* ADCx, uint8_t ADC_Channel);

	static void Initialize();
	static void DMAInit();
	static void read();
};

#endif /* ADC_H_ */
