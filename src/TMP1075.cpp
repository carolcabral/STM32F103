/*
 * TMP1075.cpp
 *
 *  Created on: Jul 25, 2020
 *      Author: carol
 */

#include "TMP1075.h"

extern  EXTI_HandleTypeDef hEXTI_TMP;
extern I2C_HandleTypeDef I2CHandle;

TMP1075::TMP1075(uint8_t address) {
	this->i2c_address = address;
}

void TMP1075::Initialize(I2C_TypeDef* I2Cx, uint32_t i2c_speed){

	I2CHandle.Instance = I2Cx;
	I2CHandle.Init.ClockSpeed = i2c_speed; //100000;
	I2CHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
	I2CHandle.Init.OwnAddress1 = 0;
	I2CHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2CHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2CHandle.Init.OwnAddress2 = 0;
	I2CHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2CHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&I2CHandle) != HAL_OK)
	{
		Error_Handler();
	}
}

float TMP1075::readTemperature(){
	uint8_t buf[3];
	int16_t value;
	float temperature;


	buf[0] = TEMP_REGISTER;
	if (HAL_I2C_Master_Transmit(&I2CHandle, this->i2c_address, buf, 1, HAL_MAX_DELAY) != HAL_OK) {
		Error_Handler();
	} else {
		if (HAL_I2C_Master_Receive(&I2CHandle, this->i2c_address, buf, 2, HAL_MAX_DELAY) != HAL_OK) {
			Error_Handler();
		} else {
			value = ((int16_t) buf[0] << 4) | (buf[1] >> 4);
			if (value > 0x7FF) {
				value |= 0xF000;
			}
			temperature = value * 0.0625;
		}
	}
	return temperature;
}

void TMP1075::configAlertPin(uint8_t exti_gpio){
	portMode(TMP1075_ALERT, GPIO_PIN_2, GPIO_MODE_INPUT);
	configIT(&hEXTI_TMP, exti_gpio);

}

void EXTI2_IRQHandler(){
	HAL_NVIC_ClearPendingIRQ(EXTI2_IRQn);
	Error_Handler();

}
