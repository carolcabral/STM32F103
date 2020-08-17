/**
 ******************************************************************************
 * @file		i2c.h
 * @author	Yohanes Erwin Setiawan
 * @date		9 March 2016
 ******************************************************************************
 */

#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/** Includes ---------------------------------------------------------------- */
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "Error.h"
#include <assert.h>
#include <unistd.h>
/** Defines ----------------------------------------------------------------- */


/*			SDA		SCL
 * I2C1		B9		B8
 * 			B7		B6
 *
 * I2C2		B11		B10
 */


class I2C{

public:
	uint32_t 		I2Cx_RCC;
	I2C_TypeDef* 	I2Cx;
	uint32_t 		I2C_GPIO_RCC;
	GPIO_TypeDef* 	I2C_GPIOx;
	uint16_t 		I2C_PIN_SDA;
	uint16_t 		I2C_PIN_SCL;

	I2C();
	//BUG - I don't know the reason, but there are some valid clock
	//that generate a hardware.
	bool Initialization(I2C_TypeDef* I2Cx, uint32_t clock);


	bool Reset(void);

	bool I2Cwrite(uint8_t address, uint8_t *data, uint8_t len);
	bool I2Cread(uint8_t address, uint8_t *data, uint8_t len);

private:
	bool sendStart();
	bool sendStop();
	bool addressDirection(uint8_t address, uint8_t direction);
	bool sendByte(uint8_t byte);
	bool receiveAck(uint8_t &data);
	bool receiveNAck(uint8_t &data);



};

#ifdef __cplusplus
}
#endif

#endif

/********************************* END OF FILE ********************************/
/******************************************************************************/
