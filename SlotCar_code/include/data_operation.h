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
#include <math.h>

// DEFINITIONS
    /* Correlation */
// for test ride use DP=200, CW=300, SW=1
#define DUMP_SAMPLES 0                      // Specify number of fed samples into the algorithm to be dumped at the beginning
#define CORRELATION_WINDOW 100              // Number of samples for reference lap
#define SLIDING_WINDOW 3                    // Number of samples to slide in the window
#define CORRELATION_THRESHOLD 500           // CORRELATION_THRESHOLD for cross-correlation to detect a new lap
#define CORRELATION_MATCH_THRESHOLD 100     // exactMatch threshold
#define CORRELATION_SAD_THRESHOLD 1000      // SAD threshold, the close to 0 the higher the match // consider setting lower
#define CORRELATION_MIN_POSITIVE 4          // Set minimum number for positive correlations


    /* DEBUG */
//#define CORR_BLE_DBG_REGISTERS              // comment out to disable BLE debug registers messages
//#define CORR_BLE_DBG_CORSUM                 // comment out to disable BLE debug correlation sums messages
//#define CORR_BLE_DBG_SAMP_COUNT             // comment out to disable BLE debug corr sample counting messages

    /* Convolution */
#define CONV_KERNEL_SIZE 8  // Define based on expected pattern size
#define CONV_THRESHOLD -2000 // Example CORRELATION_THRESHOLD; adjust based on testing
#define CONV_DEBOUNCE_COUNT 3  // Number of consistent readings before state change

// VARIABLES

// FUNCTIONS
bool corrPerform(const uint16_t *referenceData, const uint16_t *correlationData, uint16_t windowSize);
bool corrDetectNewLapStart(uint16_t newADCValue);
void corrClearBuffers(void);
bool corrExactMatch(const uint16_t *referenceData, const uint16_t *correlationData, uint16_t windowSize);
bool corrSAD(const uint16_t *referenceData, const uint16_t *correlationData, uint16_t windowSize);

int16_t convPerform(uint16_t* data, uint32_t length, uint32_t index);
void convAnalyzeKernel(uint16_t* data, uint32_t length);

#endif /* DATA_OPERATION_H_ */
