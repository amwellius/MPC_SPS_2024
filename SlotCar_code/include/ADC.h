/*
 * ADC.h
 *
 *  Created on: 07/October/2024
 *      Author: xkosik09
 */

#ifndef ADC_H_
#define ADC_H_

// INCLUDES
#include "stdint.h"
#include <msp430.h>

// DEFINITIONS
#define FILTER_WINDOW_SIZE 1000  // Window for moving average filter
#define filter_ON      // Comment this out to disable the filter

// FUNCTIONS
void ADC_init(void);
void ADC_start(void);
uint16_t ADC_get_result(uint8_t index);  // Declare the getter
int32_t moving_average(void);


#endif /* ADC_H_ */
