/*
 * TMP1075.cpp
 *
 *  Created on: 4 de abr de 2020
 *      Author: carol
 */

#include "TMP1075.h"

I2C_TypeDef* 	TMP1075::I2Cx;

TMP1075::TMP1075(uint8_t address) {
	// TODO Auto-generated constructor stub
	this->i2c_address = address;

}

bool TMP1075::Initialize(I2C_TypeDef* I2Cx, uint32_t i2c_speed){
	TMP1075::I2Cx = I2Cx;
	return I2CHandleTemperature.Initialization(TMP1075::I2Cx, i2c_speed);
//50000
}
/**
 ******************************************************************************
 *	@brief	Configure TMP1075
 * @param	Mode: Specifies if One-Shot conversion (MODE_OS) or Shutdow mode (MODE_SD),
 * 				otherwise, continuous mode
 * 	@param  Low: Low temperature limit
 * 	@param High: High temperature limit
 * 	@param [MISSING] FaulTrigger,
 * @retval	true
 ******************************************************************************
 */


bool TMP1075::config(uint8_t mode, uint16_t low, uint16_t high){

	uint8_t TxBuffer[3];

	TxBuffer[0] = LLIM_REGISTER;
	TxBuffer[1] = (uint8_t) (low >> 8); //MSB
	TxBuffer[2] = (int8_t) low; //LSB
	I2CHandleTemperature.I2Cwrite(i2c_address, TxBuffer, 3);

	TxBuffer[0] = HLIM_REGISTER;
	TxBuffer[1] = (uint8_t) (high >> 8); //MSB
	TxBuffer[2] = (int8_t) high; //LSB
	I2CHandleTemperature.I2Cwrite(i2c_address, TxBuffer, 3);

	TxBuffer[0] = CFGR_REGISTER;
	TxBuffer[1] = mode; //MSB
	I2CHandleTemperature.I2Cwrite(i2c_address, TxBuffer, 2);

	return true;
}

float TMP1075::readTemperature(){
	uint8_t RxBuffer[2];
	uint16_t value;

	RxBuffer[0] = CFGR_REGISTER;
	RxBuffer[1] = MODE_OS;
	I2CHandleTemperature.I2Cwrite(this->i2c_address, RxBuffer, 2);

	I2CHandleTemperature.I2Cwrite(this->i2c_address, (uint8_t *) TEMP_REGISTER, 1);
	I2CHandleTemperature.I2Cread(this->i2c_address, RxBuffer, 2);

	value = ((uint16_t) RxBuffer[0]) << 4 | ((uint16_t) (RxBuffer[1]) >> 4);
	if (value > 0x7FF){
		value |= 0xF000;
	}

	return value * 0.0625;
}

uint16_t TMP1075::returnID(){
	uint8_t RxBuffer[2];

	I2CHandleTemperature.I2Cwrite(i2c_address, (uint8_t *) DIEID_REGISTER, 1);
	I2CHandleTemperature.I2Cread(i2c_address, RxBuffer, 2);

	return ((uint16_t) RxBuffer[0]) << 4 | ((uint16_t) (RxBuffer[1]) >> 4);
}
