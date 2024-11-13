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
#include <stdio.h>
#include <string.h>
#include "include/UART.h"

// DEFINITIONS
    /* Correlation */
//example: CORRELATION_WINDOW 20, SLIDING_WINDOW 1, CORRELATION_THRESHOLD 500, DUMP_SAMPLES 110, dataset3
#define CORRELATION_WINDOW 300              // Number of samples for reference lap //104
#define SLIDING_WINDOW 1                  // Number of samples to slide in the window //26
#define CORRELATION_THRESHOLD 500  // CORRELATION_THRESHOLD for cross-correlation to detect a new lap
#define DUMP_SAMPLES 200                  // Specify number of fed samples into the algorithm to be dumped at the beginning
//#define CORR_BLE_DBG_REGISTERS          // comment out to disable BLE debug registers messages
#define CORR_BLE_DBG_CORSUM             // comment out to disable BLE debug correlation sums messages
//#define CORR_BLE_DBG_SAMP_COUNT         // comment out to disable BLE debug corr sample counting messages

    /* Convolution */
#define CONV_KERNEL_SIZE 8  // Define based on expected pattern size
#define CONV_THRESHOLD -2000 // Example CORRELATION_THRESHOLD; adjust based on testing
#define CONV_DEBOUNCE_COUNT 3  // Number of consistent readings before state change

// VARIABLES

// FUNCTIONS
bool corrPerform(const uint16_t *referenceData, const uint16_t *correlationData, uint16_t windowSize);
bool corrDetectNewLapStart(uint16_t newADCValue);
void corrClearBuffers(void);

int16_t convPerform(uint16_t* data, uint32_t length, uint32_t index);
void convAnalyzeKernel(uint16_t* data, uint32_t length);



#endif /* DATA_OPERATION_H_ */
