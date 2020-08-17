/*
 *  Created on: 11 de abr de 2020 by Ana Carolina Cabral
 *  
 */

#include "ADC.h"

uint16_t 		ADC::ADC_Values[__NCHANNELS];
uint8_t 		ADC::nchannel=0;
uint16_t 		ADC::ADC_PIN;
ADC_TypeDef* 	ADC::ADCx;
uint32_t 		ADC::ADC_RCC = 0;
uint32_t 		ADC::ADC_GPIO_RCC = 0;
uint8_t 		ADC::ADC_CH[__NCHANNELS];
GPIO_TypeDef*   ADC::ADC_GPIO;

/******************************************************************************
 *	@brief	Instantiate each channel
 * @param	ADCx: Instance of ADC1 or ADC2 (ADC2 does not support DMA)
 * @param 	ADC_Channel: which ADC Channel to convert (ADC_Channel_0 to ADC_Channel_7)
 * @retval 	None
 ******************************************************************************/

ADC::ADC(ADC_TypeDef* ADCx, uint8_t ADC_Channel) {
	// TODO Auto-generated constructor stub

	ADC::ADC_CH[nchannel] = ADC_Channel;
	ADC::nchannel++;

	ADC::ADCx = ADCx;

	if (ADCx == ADC1) this->ADC_RCC = RCC_APB2Periph_ADC1;
	else this->ADC_RCC = RCC_APB2Periph_ADC2;

	ADC::ADC_PIN = GPIO_Pin_0 << ADC_Channel;

	if (ADC_Channel == ADC_Channel_8 || ADC_Channel == ADC_Channel_9 ){
		ADC::ADC_GPIO = GPIOB;
		ADC::ADC_GPIO_RCC |= RCC_APB2Periph_GPIOB;
	} else {
		ADC::ADC_GPIO = GPIOA;
		ADC::ADC_GPIO_RCC |= RCC_APB2Periph_GPIOA;
	}
	/*	2 ADC - 10 Channels
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
	 * PB1		ADC IN9*/
}


/******************************************************************************
 *	@brief	Initialize ADCx
 *		*Currently only with Channel0 and Channel6
 * @param 	None
 * @retval 	None
 ******************************************************************************/


void ADC::Initialize(){

	//ADC Prescaler
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);// PCLK is the APB2 clock (72MHz) / 6 = 12MHz (max 14MHz)

	//Enable GPIO and ADCx clock and configure pins
	RCC_APB2PeriphClockCmd(ADC::ADC_RCC | ADC::ADC_GPIO_RCC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = ADC::ADC_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ADC::ADC_GPIO, &GPIO_InitStruct);

	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent; //ADC1 and ADC2 operate independently
	ADC_InitStruct.ADC_ScanConvMode = ENABLE; //Scan multiple channels
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE; //Continuous conversion
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //Software start
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right; //12 bit right aligned
	ADC_InitStruct.ADC_NbrOfChannel = ADC::nchannel;
	ADC_Init(ADC::ADCx, &ADC_InitStruct);

	for ( volatile int i = 0; i < ADC::nchannel; i++){
		ADC_RegularChannelConfig(ADC::ADCx, ADC::ADC_CH[i], ++i, ADC_SampleTime_7Cycles5);
	}
	//ADC_RegularChannelConfig(ADCx, ADC_Channel_0, 1, ADC_SampleTime_7Cycles5);
	//ADC_RegularChannelConfig(ADCx, ADC_Channel_6, 2, ADC_SampleTime_7Cycles5);

	ADC_Cmd(ADC::ADCx, ENABLE);

	//Enable DMA request
	ADC_DMACmd(ADC::ADCx, ENABLE);
	ADC::DMAInit();

	/*Calibration*/
	ADC_Cmd(ADC::ADCx, ENABLE);
	ADC_ResetCalibration(ADC::ADCx);
	while (ADC_GetResetCalibrationStatus(ADC::ADCx));
	ADC_StartCalibration(ADC::ADCx);
	while (ADC_GetResetCalibrationStatus(ADC::ADCx));
}

/******************************************************************************
 *	@brief	Initialize DMA1 Channel 1
 * @param	None
 * @retval 	ADC::ADC_Valuess returns ADC converted data for n_ADCCHANNELS;
 ******************************************************************************/

void ADC::DMAInit(){
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //memory to memory?
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = ADC::nchannel;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(ADC1->DR));
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) (ADC::ADC_Values);

	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);


	DMA_Cmd(DMA1_Channel1, ENABLE);

}

/******************************************************************************
 *	@brief	Read value (SCAN mode with DMA)
 * @param	None
 * @retval 	None
 ******************************************************************************/


void ADC::read(){
	DMA_Cmd(DMA1_Channel1, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	ADC_Cmd(ADC::ADCx, ENABLE);


	ADC_SoftwareStartConvCmd(ADC::ADCx, ENABLE);
}


