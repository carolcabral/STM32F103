/*
 * My_Macros.h
 *
 *  Created on: Jan. 8, 2020
 *      Author: educampos
 */

#ifndef MY_MACROS_H_
#define MY_MACROS_H_

#define WHILE_COUNT(__X)	\
	{	\
		uint16_t __Counter=0;	\
		while(__X)	\
		{	\
			usleep(1000);	\
			__Counter++;	\
			if (__Counter>=1000)	\
				return false;	\
		}	\
	}

//the SPI is a shared resource, therefore, to avoid the
//interruption while a message is being sent
//this mutex MUST be used
//portMAX_DELAY = delay indefinitely
#define MUTEX_SPI(__Y)	\
	{	\
		xSemaphoreTake(xSPI_Semaphore, portMAX_DELAY);	\
		__Y;\
		xSemaphoreGive(xSPI_Semaphore);         \
	}


#define __disable_irq() asm volatile ("cpsid i" : : : "memory")
#define __enable_irq()  asm volatile ("cpsie i" : : : "memory")

//#ifdef FREERTOS_CONFIG_H
//	#define assert(_X) configASSERT(_X)
	#include "stdio.h"
	#include "diag/Trace.h"
	#undef assert
    #define assert(_X){\
		if( ( _X ) == 0 )\
		{\
			__disable_irq();\
			uint8_t* buf = new uint8_t[100];\
			size_t ret = sprintf((char*)buf,"Assert called on File %s, Line %i.\n", __FILE__, __LINE__ );\
			trace_write((char*)buf,(size_t)ret);\
			abort ();\
		}\
	}




//#else
//	#include <assert.h>
//	#include "diag/Trace.h"
//#endif


#endif /* MY_MACROS_H_ */
