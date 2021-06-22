/*
 * adc.h
 *
 *  Created on: 10 apr 2019
 *      Author: stefo
 */

#ifndef SRC_ADC_H_
#define SRC_ADC_H_

#include "LPC8xx.h"

void InitADC(void);
uint16_t ReadADC(void);

#endif /* SRC_ADC_H_ */
