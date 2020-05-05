/*
 * TIMER.h
 *
 *  Created on: 5 de mai de 2020
 *      Author: carol
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "stm32f10x.h"


void delay (int us);

class TIMER {
public:
	TIMER();

	void Initialize(void);
};



#endif /* TIMER_H_ */
