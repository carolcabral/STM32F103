/*
 * TMP1075.h
 *
 *  Created on: 4 de abr de 2020
 *      Author: carol
 */

#ifndef TMP1075_H_
#define TMP1075_H_

#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"

#include "My_Macros.h"
#include "i2c.h"

static const uint16_t TEMP_REGISTER = 0x00;
static const uint16_t CFGR_REGISTER = 0x01;
static const uint16_t LLIM_REGISTER = 0x02;
static const uint16_t HLIM_REGISTER = 0x03;
static const uint16_t DIEID_REGISTER = 0x0F;

/* CFGR_REGISTER: only MSB byte is used
 * MODE_OS | CONV_RATE | FAULT_TRIGGER | POL_HIGH | IT_MODE | MODE_SD
 * */
static const uint8_t MODE_SD = 1 << 0; //Otherwise continuous mode
static const uint8_t IT_MODE = 1 << 1; //Otherwise comparator mode
static const uint8_t POL_HIGH = 1 << 2; //Otherwise polarity active low
static const uint8_t FAULT_2 = 1 << 3;
static const uint8_t FAULT_3 = 1 << 4;
static const uint8_t FAULT_4 = (1 << 3)  | (1 << 4);
static const uint8_t CONV_RATE_55 = 1 << 5;
static const uint8_t  CONV_RATE_110 = (1 << 6);
static const uint8_t  CONV_RATE_220 = (1 << 5)  | (1 << 6);
static const uint8_t MODE_OS = 1 << 7;


/*
 * LLIM and HLIM only uses 12 MSB bits in two complements
 */


static I2C I2CHandleTemperature;
//extern I2C I2CHandleTemperature;

class TMP1075 {
public:
	uint8_t					i2c_address;
	static I2C_TypeDef* 	I2Cx;


	TMP1075(uint8_t address);

	static bool Initialize(I2C_TypeDef* I2Cx, uint32_t i2c_speed);
	bool config(uint8_t mode, uint16_t low, uint16_t high);
	float readTemperature();
	uint16_t returnID();

};

#endif /* TMP1075_H_ */
