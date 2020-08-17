/**
 * @file SDCard.cpp
 * 
 * @author
 * Angelo Elias Dalzotto (150633@upf.br)
 * GEPID - Grupo de Pesquisa em Cultura Digital (http://gepid.upf.br/)
 * Universidade de Passo Fundo (http://www.upf.br/)
 * 
 * @copyright
 * Copyright (C) 2009 by William Greiman
 * 
 * @brief This is a pure AVR port of the Arduino Sd2Card Library.
 * This file has the low-level disk I/O with SD Cards connected to SPI.
 * 
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino Sd2Card Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "SDCard.h"


// Port numbers: 0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G, ...
SDCard::SDCard(volatile uint8_t PORT_CS, uint8_t PIN_CS, SPI *dev)
{
    error = Error_Type::OK;
    in_block = 0;
    offset = 0;
    status = 0;
    block = 0;
    partial_block_read = 0;

    // Enable GPIO Peripheral clock
    RCC_APB2PeriphClockCmd(RCC_MASKx(PORT_CS), ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    // Configure pin CS
    GPIO_InitStructure.GPIO_Pin = PIN_MASK(PIN_CS);
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOx_MASK(PORT_CS), &GPIO_InitStructure);

    this->PORT_CS = PORT_CS;
    this->PIN_CS = PIN_CS;

    deselect();

    spi = dev;

    //to ensure the proper operation of the SD card,
    //the SD CLK signal should have a frequency in the
    //range of 100 to 400 kHz.
}

bool SDCard::init()
{
    Millis::init();
    error = Error_Type::OK;
    in_block = partial_block_read = 0;
    
    deselect();

    usleep(10000);
    // 80 Dummy SPI clocks
    for(int i = 0; i < 10; i++)
        spi->write(0xFF);
    
    usleep(10000);
    // Selects chip
    select();

    uint32_t then = Millis::get();

    // Put SD Card in idle mode
    while ((status = send_cmd(CMD0, 0)) != R1_IDLE_STATE) {
        uint16_t diff = Millis::get() - then;
        if (diff > SD_INIT_TIMEOUT) {
            error = Error_Type::CMD0;
            deselect();
            return false;
        }
    }

    // Check SD type
    //This command is only available in the latest cards,
    //compatible with SDcard Specifications version 2.0.
    const uint8_t checkPatter = 0xA5;
    const uint16_t voltage = 0x0100; //2.7V to 3.6V
    if(send_cmd(CMD8, checkPatter | voltage ) & R1_ILLEGAL_COMMAND){
        type = Type::SDv1; //old card version

    }
    else
    {
        // only need last byte of r7 response
    	uint8_t buff[4]={0xFF,0xFF,0xFF,0xFF};
    	spi->write_read_buffer(buff,4,buff);

    	//verify if the patter echo
        if (buff[3] != checkPatter)
        {
            error = Error_Type::CMD8;
            deselect();
            return false;
        }

        type = Type::SDv2;
    }

    // initialize card and send host supports SDHC if SD2
    volatile uint32_t arg = type == Type::SDv2 ? 0X40000000 : 0;

    status = send_acmd(CMD58, 0);

	uint8_t buff[4]={0xFF,0xFF,0xFF,0xFF};
	spi->write_read_buffer(buff,4,buff);

	//voltage error
	if( (buff[1]!=0xFF) && ((buff[2]&0x80)!=0x80)){
        error = Error_Type::CMD58;
        deselect();
        return false;
    }

    while ((status = send_acmd(ACMD41, arg)) != R1_READY_STATE) {

    	uint8_t buff[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    	spi->write_read_buffer(buff,6,buff);

        // Check for timeout
        uint16_t diff = Millis::get() - then;
        if (diff > SD_INIT_TIMEOUT) {
            error = Error_Type::ACMD41;
            deselect();
            return false;
        }

        if (status & R1_ILLEGAL_COMMAND){
            error = Error_Type::ILLEGAL_COMMAND;
            deselect();
            assert(false);
            return false;
        }

    }

    // if SD2 read OCR register to check for SDHC card
    if (type == Type::SDv2) {
        if (send_cmd(CMD58, 0)) {
            error = Error_Type::CMD58;
            deselect();
            return false;
        }
        if ((spi->write_read(0xFF) & 0XC0) == 0XC0) type = Type::SDHC;
        // discard rest of ocr - contains allowed voltage range
        for (uint8_t i = 0; i < 3; i++) spi->write_read(0xFF);
    }

    deselect();
    return true;
}

void SDCard::deselect()
{

    GPIO_SetBits(GPIOx_MASK(PORT_CS),PIN_MASK(PIN_CS));
}

void SDCard::select()
{
	GPIO_ResetBits(GPIOx_MASK(PORT_CS),PIN_MASK(PIN_CS));
}

static uint8_t CRC7_one(uint8_t t, uint8_t data) {
	const uint8_t poly = 0x89;  // the value of our CRC-7 polynomial
	uint8_t i;

	t ^= data;
	for (i = 0; i < 8; i++) {
		if (t & 0x80) t ^= poly;
		t <<= 1;
	}

	return t;
}

uint8_t CRC7_buf(const uint8_t * p, uint8_t len) {
	uint8_t j,crc = 0;

	for (j = 0; j < len; j++) crc = CRC7_one(crc,p[j]);

	return crc >> 1;
}

uint8_t SDCard::send_cmd(uint8_t cmd, uint32_t arg)
{
//    end_read();

	uint8_t commandSequence[] = { (uint8_t) (cmd | 0x40),
								  (uint8_t) (arg >> 24),
								  (uint8_t) (arg >> 16),
								  (uint8_t) (arg >> 8),
								  (uint8_t) (arg & 0xFF),
								  0xFF };

	select();
    wait_busy(300);

    uint8_t crc = CRC7_buf(commandSequence,5);
    commandSequence[5]= (crc<<1) | 0x01;

    spi->writeBytes(commandSequence,6);

    //If the expected response is not received within 16 clock cycles after
    //sending the reset command, the reset command has to be sent again.
    //The first bit is always a 0, then use that to identify that a status was received
//    for (uint8_t i = 0; ((status = spi->read()) & 0x80) && i != 0x20; i++);
    uint8_t i=0;
    while(i!=20)
    {
    	status = spi->read();

    	//The first bit is always a 0, then use that to identify that a status was received
    	if (~status & 0x80)
    		break;

    	i++;
    }

    return status;
}

void SDCard::end_read()
{
    if(in_block){
        while(offset++ < 514) spi->read();
        deselect();
        in_block = 0;
    }
}

bool SDCard::wait_busy(uint32_t milliseconds)
{
    uint32_t then = Millis::get();
    do {
        if(spi->read() == 0xFF)
            return true;
    } while(Millis::get() - then < milliseconds);
    return false;
}

uint8_t SDCard::send_acmd(uint8_t cmd, uint32_t arg)
{
    send_cmd(CMD55, 0);
    return send_cmd(cmd, arg);
}

SDCard::Type SDCard::get_type()
{
    return type;
}

SDCard::Error_Type SDCard::get_error()
{
    return error;
}

bool SDCard::write_block(uint32_t block_no, const uint8_t* src)
{
    // don't allow write to first block
    if (!block_no) {
        error = Error_Type::WRITE_BLOCK_ZERO;
        deselect();
        return false;
    }

    // use address if not SDHC card
    if(type != Type::SDHC) block_no <<= 9;

    if(send_cmd(CMD24, block_no)){
        error = Error_Type::CMD24;
        deselect();
        return false;
    }

    if(!write_data(DATA_START_BLOCK, src)){
        deselect();
        return false;
    }

    // wait for flash programming to complete
    if(!wait_busy(SD_WRITE_TIMEOUT)) {
        error = Error_Type::WRITE_TIMEOUT;
        deselect();
        return false;
    }

    // response is r2 so get and check two bytes for nonzero
    if(send_cmd(CMD13, 0) || spi->read()) {
        error = Error_Type::WRITE_PROGRAMMING;
        deselect();
        return false;
    }
    
    deselect();
    return true;
}

bool SDCard::write_data(uint8_t token, const uint8_t* src)
{
    spi->write(token);
    for (uint16_t i = 0; i < 512; i++) {
        spi->write(src[i]);
    }

    spi->write(0xff);  // dummy crc
    spi->write(0xff);  // dummy crc

    status = spi->read();
    if((status & DATA_RES_MASK) != DATA_RES_ACCEPTED){
        error = Error_Type::WRITE;
        deselect();
        return false;
    }
    return true;
}

bool SDCard::read_block(uint32_t block, uint8_t *dst)
{
    return read_data(block, 0, 512, dst);
}

bool SDCard::read_data(uint32_t block, uint16_t offset, uint16_t count, uint8_t *dst)
{
    if(!count)
        return true;

    if((count + offset) > 512){
        deselect();
        return false;
    }

    if(!in_block || block != this->block || offset < this->offset){
        this->block = block;
            // use address if not SDHC card
        if(type != Type::SDHC) block <<= 9;
        if(send_cmd(CMD17, block)){
            error = Error_Type::CMD17;
            deselect();
            return false;
        }
        if(!wait_start_block()) {
            deselect();
            return false;
        }
        this->offset = 0;
        in_block = 1;
    }

    // skip data before offset
    for (;this->offset < offset; this->offset++) {
        spi->read();
    }
    // transfer data
    for (uint16_t i = 0; i < count; i++) {
        dst[i] = spi->read();
    }

    this->offset += count;
    if (!partial_block_read || this->offset >= 512) {
        // read rest of data, checksum and set chip select high
        end_read();
    }
    return true;
}

bool SDCard::wait_start_block()
{
    uint32_t then = Millis::get();
    while((status = spi->read()) == 0XFF){
        uint16_t diff = Millis::get() - then;
        if(diff > SD_READ_TIMEOUT){
            error = Error_Type::READ_TIMEOUT;
            deselect();
            return false;
        }
    }
    if (status != DATA_START_BLOCK) {
        error = Error_Type::READ;
        deselect();
        return false;
    }
    return true;
}




