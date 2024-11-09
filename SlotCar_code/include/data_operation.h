/*
 * data_operation.h
 *
 *  Created on: 09/November/2024
 *      Author: xkosik09
 */

#ifndef DATA_OPERATION_H_
#define DATA_OPERATION_H_

// INCLUDES
#include <stdint.h>
#include <stdbool.h>
#include "include/UART.h"

// DEFINITIONS
    /* Correlation */
//example: CORRELATION_WINDOW 21, SLIDING_WINDOW 7, CORRELATION_THRESHOLD 19334000
#define CORRELATION_WINDOW 21           // Number of samples for reference lap
#define SLIDING_WINDOW 7                // Number of samples to slide in the window
#define CORRELATION_THRESHOLD 19334000  // CORRELATION_THRESHOLD for cross-correlation to detect a new lap
//#define CORR_BLE_DBG // comment out to disable BLE debug messages

    /* Convolution */
#define CONV_KERNEL_SIZE 8  // Define based on expected pattern size
#define CONV_THRESHOLD -2000 // Example CORRELATION_THRESHOLD; adjust based on testing
#define CONV_DEBOUNCE_COUNT 3  // Number of consistent readings before state change

// VARIABLES

// FUNCTIONS
bool corrDetectNewLapStart(const uint16_t *referenceData, const uint16_t *correlationData, uint16_t windowSize);
void corrCollectADCData(uint16_t newADCValue);

int16_t perform_convolution(uint16_t* data, uint32_t length, uint32_t index);
void analyze_track_section(uint16_t* data, uint32_t length);



#endif /* DATA_OPERATION_H_ */
