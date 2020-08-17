/*
 * SDCard.h
 *
 *  Created on: Jul 26, 2020
 *      Author: carol
 */

#ifndef SRC_SDCARD_H_
#define SRC_SDCARD_H_


#include "main.h"
#include "fatfs.h"

uint32_t freeSpaceSD();
uint8_t unmountSD();
uint8_t mountSD();
uint8_t readSD(const char* file, uint8_t *buffer);
uint8_t deleteFile(const char* file);
uint8_t writeSD(const char *file, char *data);

#endif /* SRC_SDCARD_H_ */
