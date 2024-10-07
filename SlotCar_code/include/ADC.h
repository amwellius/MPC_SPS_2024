/*
 * ADC.h
 *
 *  Created on: 07/October/2024
 *      Author: xkosik09
 */

#ifndef ADC_H_
#define ADC_H_


#include "stdint.h"
#include <msp430.h>
// macros definition


// functions prototypes
void ADC_init(void);
void ADC_start(void);
uint16_t ADC_get_result(uint8_t index);  // Declare the getter


// variables

#endif /* ADC_H_ */
