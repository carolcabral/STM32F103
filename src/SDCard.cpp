/*
 * SDCard.cpp
 *
 *  Created on: Jul 26, 2020
 *      Author: carol
 */


#include "SDCard.h"

FATFS fs;  // file system
FIL fil;  // file
FRESULT fresult;  // to store the result

UINT br, bw;   // file read/write count

/* capacity related variables */
FATFS *pfs;
DWORD fre_clust;


uint8_t writeSD(const char *file, char *data){
	/* Open file to write/ create a file if it doesn't exist */
	fresult = f_open(&fil, file, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

	/* Move to offset to the end of the file */
	fresult = f_lseek(&fil, fil.fsize);

	/* Writing text */
	f_puts(data, &fil);

	/* Close file */
	fresult = f_close(&fil);

	return SUCCESS;
}


uint8_t deleteFile(const char* file){
	fresult = f_unlink(file);
	if (fresult == FR_OK) return SUCCESS;
	else return ERROR;

}

uint8_t readSD(const char* file, uint8_t *buffer){
	/* Open to read the file */
	fresult = f_open (&fil, file, FA_READ);

	/* Read string from the file */
	f_read (&fil, buffer, fil.fsize, &br);

	/* Close file */
	f_close(&fil);

	return SUCCESS;
}


uint8_t mountSD(){
	fresult = f_mount(&fs, "", 0);
	if (fresult == FR_OK) return SUCCESS;
	else return ERROR;

}

uint8_t unmountSD(){
	fresult = f_mount(NULL, "", 1);
	if (fresult == FR_OK) return SUCCESS;
	else return ERROR;

}

uint32_t freeSpaceSD(){
	uint32_t total, free_space;
	/* Check free space */
	f_getfree("", &fre_clust, &pfs);
	total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
	free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
	return free_space;
}

