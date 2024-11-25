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
#define filter_ON       // Comment this out to disable the filter
#define frame_ON        // Comment this out to disable framing the samples

#define ADC_UPPER_FRAME 1988        // upper frame
#define ADC_LOWER_FRAME 1920        // lower frame

// FUNCTIONS
void ADC_init(void);                        // inits the ADC
void ADC_start(void);                       // starts ADC conversions
void ADC_stop(void);                        // stops ADC. Needed to call ADC_init and ADC_start to start the ADC again
uint16_t ADC_get_result(uint8_t index);     // Declare the getter
int32_t moving_average(void);
int32_t frame_samples(void);


#endif /* ADC_H_ */
