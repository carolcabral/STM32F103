/**
 ******************************************************************************
 * @file		i2c.c
 * @author	Yohanes Erwin Setiawan
 * @date		9 March 2016
 *
 *
 *
 * @modified 4 april 2020
 ******************************************************************************
 */

/** Includes ---------------------------------------------------------------- */
#include "i2c.h"

#define while_timeout(__X)		\
		{							\
	uint32_t __Counter=2000000;	\
	while(__X)				\
	{						\
		__Counter--;		\
		if (__Counter==0){	\
			return Error; 	\
		}					\
	}						\
		}


I2C::I2C(){
	I2Cx=0;
}

/** Public functions -------------------------------------------------------- */
/**
 ******************************************************************************
 *	@brief	Initialize I2C in master mode
 * @param	None
 * @retval	None
 ******************************************************************************
 */
bool I2C::Initialization(I2C_TypeDef* I2Cx, uint32_t clock){

	this->I2Cx = I2Cx;
	if (I2Cx == I2C1)
	{
		I2Cx_RCC			=RCC_APB1Periph_I2C1;
		I2C_GPIO_RCC		=RCC_APB2Periph_GPIOB;
		I2C_GPIOx			=GPIOB;
		I2C_PIN_SDA			=GPIO_Pin_7;
		I2C_PIN_SCL			=GPIO_Pin_6;
	}
	else if (I2Cx == I2C2)
	{
		I2Cx_RCC			=RCC_APB1Periph_I2C2;
		I2C_GPIO_RCC		=RCC_APB2Periph_GPIOB;
		I2C_GPIOx			=GPIOB;
		I2C_PIN_SDA			=GPIO_Pin_11;
		I2C_PIN_SCL			=GPIO_Pin_10;
	} else
	{
		assert(false);
	}

	//	Fail();
	// Initialization struct
	I2C_InitTypeDef I2C_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;

	// Step 2: Initialize GPIO as open drain alternate function
	//RCC_APB2PeriphResetCmd(I2C_GPIO_RCC,ENABLE);
	//RCC_APB2PeriphResetCmd(I2C_GPIO_RCC,DISABLE);

	RCC_APB2PeriphClockCmd(I2C_GPIO_RCC, ENABLE);
	GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL | I2C_PIN_SDA;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIOx, &GPIO_InitStruct);

	// Step 1: Initialize I2C
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = clock ;

	/* I2C1 Reset */
	RCC_APB1PeriphResetCmd(I2Cx_RCC,ENABLE);

	RCC_APB1PeriphResetCmd(I2Cx_RCC,DISABLE);
	RCC_APB1PeriphClockCmd(I2Cx_RCC, ENABLE);
	I2C_DeInit(I2Cx);
	I2C_Init(I2Cx, &I2C_InitStruct);
	I2C_Cmd(I2Cx, ENABLE);

	return true;
}

/**
 ******************************************************************************
 *	@brief	Reset the I2C bus
 * @param	None
 * @retval
 * //https://www.st.com/content/ccc/resource/technical/document/errata_sheet/7d/02/75/64/17/fc/4d/fd/CD00190234.pdf/files/CD00190234.pdf/jcr:content/translations/en.CD00190234.pdf
 *
 ******************************************************************************
 */

bool I2C::Reset(void)
{

	//Disable the I2C peripheral by clearing the PE bit in I2Cx_CR1 register.
	CLEAR_BIT(I2Cx->CR1,I2C_CR1_PE);
	asm("nop");
	RCC_APB1PeriphClockCmd(I2Cx_RCC, DISABLE);

	//2- Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level
	RCC_APB2PeriphClockCmd(I2C_GPIO_RCC, ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL|I2C_PIN_SDA;// | I2C_PIN_SDA;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(I2C_GPIOx, &GPIO_InitStruct);

	GPIO_SetBits(I2C_GPIOx, I2C_PIN_SCL|I2C_PIN_SDA);

	//3 - Check SCL and SDA High level in GPIOx_IDR.
	if (GPIO_ReadOutputDataBit(I2C_GPIOx, I2C_PIN_SCL) == Bit_RESET) {
		assert(false);
		return false; //if it is not high, return a error
	}
	if (GPIO_ReadOutputDataBit(I2C_GPIOx, I2C_PIN_SDA) == Bit_RESET) {
		assert(false);
		return false; //if it is not high, return a error
	}

	//4 -Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
	GPIO_InitStruct.GPIO_Pin = I2C_PIN_SDA;
	GPIO_Init(I2C_GPIOx, &GPIO_InitStruct);
	GPIO_ResetBits(I2C_GPIOx,I2C_PIN_SDA);

	//5 -Check SDA Low level in GPIOx_IDR.
	if (GPIO_ReadOutputDataBit(I2C_GPIOx, I2C_PIN_SDA) == Bit_SET) {
		assert(false);
		for(;;){}
	}

	// 6 Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
	GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL;
	GPIO_Init(I2C_GPIOx, &GPIO_InitStruct);
	GPIO_ResetBits(I2C_GPIOx,I2C_PIN_SCL);

	//7 Check SCL Low level in GPIOx_IDR.
	if (GPIO_ReadOutputDataBit(I2C_GPIOx, I2C_PIN_SCL) == Bit_SET) {
		assert(false);
		for(;;){}
	}

	//8 Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
	GPIO_SetBits(I2C_GPIOx,I2C_PIN_SCL);

	//9 Check SCL High level in GPIOx_IDR.
	if (GPIO_ReadOutputDataBit(I2C_GPIOx, I2C_PIN_SCL) == Bit_RESET) {
		assert(false);
		for(;;){}
	}

	//10 Configure the SDA I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
	GPIO_SetBits(I2C_GPIOx,I2C_PIN_SDA);

	//11 Check SDA High level in GPIOx_IDR.
	if (GPIO_ReadOutputDataBit(I2C_GPIOx, I2C_PIN_SDA) == Bit_RESET) {
		assert(false);
		for(;;){}
	}

	//12 Configure the SCL and SDA I/Os as Alternate function Open-Drain.
	GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL | I2C_PIN_SDA;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIOx, &GPIO_InitStruct);

	// 13. Set SWRST bit in I2Cx_CR1 register.
	SET_BIT(I2Cx->CR1,I2C_CR1_SWRST);
	asm("nop");

	/* 14. Clear SWRST bit in I2Cx_CR1 register. */
	CLEAR_BIT(I2Cx->CR1, I2C_CR1_SWRST);
	asm("nop");

	RCC_APB1PeriphClockCmd(I2Cx_RCC, ENABLE);

	/* 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register */
	SET_BIT(I2Cx->CR1,I2C_CR1_PE);
	asm("nop");

	assert(I2Cx->CR1 & I2C_CR1_PE);

	return true;

}

/**
 ******************************************************************************
 *	@brief	Write multiple bytes
 * @param	Address: slave address
 * @parameter *data: pointer to a data to be transmitted
 * @param 	Len: number of bytes to be transmitted
 * @retval	None
 ******************************************************************************
 */


bool I2C::I2Cwrite(uint8_t address, uint8_t *data, uint8_t len){
	bool status = true;

	status &= sendStart();
	status &= addressDirection(address, I2C_Direction_Transmitter);

	if (status == 0){
		sendStop();
	}


	for (int i = 0; i < len; i++){
		status &= sendByte(data[i]);
	}

	while(!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	sendStop();

	return status;
}

/**
 ******************************************************************************
 *	@brief	Read multiple bytes
 * @param	Address: slave address
 * @parameter *data: pointer to a data to be received
 * @param 	Len: number of bytes to be received
 * @retval
 ******************************************************************************
 */

bool I2C::I2Cread(uint8_t address, uint8_t *data, uint8_t len){
	bool status = true;

	status &= I2Cwrite (address, &data[0], 1);

	status &= sendStart();
	// Wait for I2C EV5.
	// It means that the start condition has been correctly released
	// on the I2C bus (the bus is free, no other devices is communicating))
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

	status &= addressDirection(address,I2C_Direction_Receiver);

	for (int i = 0; i < len-1; i++){
		while_timeout(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));
		data[i] = I2C_ReceiveData(I2Cx);
		if ( (len-i) == 1 ){
			/*enable NACK bit */
			//I2C_NACKPositionConfig(I2Cx, I2C_NACKPosition_Current);
			//I2C_AcknowledgeConfig(I2Cx, DISABLE);
			sendStop();
			status &= receiveNAck (data[i]);
		} else {
			status &= receiveAck (data[i]);
		}
	}
	return status;
}

/**
 ******************************************************************************
 *	@brief	Receive data byte from I2C bus, then return ACK
 * @param	None
 * @retval	Received data byte
 ******************************************************************************
 */

bool I2C::receiveAck(uint8_t &data){
	// Enable ACK of received data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	// Wait for I2C EV7
	// It means that the data has been received in I2C data register
	while_timeout(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

	// Read and return data byte from I2C data register
	data = I2C_ReceiveData(I2Cx);
	return true;
}

/******************************************************************************
 *	@brief	Receive data byte from I2C bus, then return ACK
 * @param	None
 * @retval	Received data byte
 ******************************************************************************
 */

bool I2C::receiveNAck(uint8_t &data){
	// Enable ACK of received data
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	// Wait for I2C EV7
	// It means that the data has been received in I2C data register
	while_timeout(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

	// Read and return data byte from I2C data register
	data = I2C_ReceiveData(I2Cx);
	return true;
}
/**
 ******************************************************************************
 *	@brief	Generate I2C start condition
 * @param	None
 * @retval	None
 ******************************************************************************
 */
bool I2C::sendStart() {
	while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	Reset();

	// Generate start condition
	I2C_GenerateSTART(I2Cx, ENABLE);

	// Wait for I2C EV5.
	// It means that the start condition has been correctly released
	// on the I2C bus (the bus is free, no other devices is communicating))

	/* check start bit flag */
	while(!I2C_GetFlagStatus(I2Cx, I2C_FLAG_SB));
	//while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
	return true;
}

/**
 ******************************************************************************
 *	@brief	Generate I2C stop condition
 * @param	None
 * @retval	None
 ******************************************************************************
 */
bool I2C::sendStop()
{
	/* Send STOP Condition */
	I2C_GenerateSTOP(I2Cx, ENABLE);
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF)); // stop bit flag
	return true;
}

/**
 ******************************************************************************
 *	@brief	Write slave address to I2C bus
 * @param	Slave address
 * @param	I2C direction (transmitter or receiver)
 * @retval	None
 ******************************************************************************
 */
bool I2C::addressDirection(uint8_t address, uint8_t direction)
{

	// Enable ACK of received data
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	// Send slave address
	I2C_Send7bitAddress(I2Cx, address, direction);


	// It means that a slave acknowledges his address
	if (direction == I2C_Direction_Transmitter)
	{
		// Wait for I2C EV8
		while_timeout(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	}
	else if (direction == I2C_Direction_Receiver)
	{
		// Wait for I2C EV6
		while_timeout (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	}

	return true;
}

/**
 ******************************************************************************
 *	@brief	Transmit one byte to I2C bus
 * @param	Data byte to transmit
 * @retval	None
 ******************************************************************************
 */
bool I2C::sendByte(uint8_t byte)
{
	// Send data byte
	I2C_SendData(I2Cx, byte);
	// Wait for I2C EV8_2.
	// It means that the data has been physically shifted out and
	// output on the bus)
	while_timeout(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	return true;
}

/********************************* END OF FILE ********************************/
/******************************************************************************/
