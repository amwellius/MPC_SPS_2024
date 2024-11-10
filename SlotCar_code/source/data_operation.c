/*
 * data_operation.c
 *
 *  Created on: 09/November/2024
 *      Author: xkosik09
 */

// INCLUDES
#include "include/data_operation.h"
#include <msp430.h>

// VARIABLES
uint16_t currentDataCounter = 0;  // Track current number of collected samples
bool referenceDataDone = false;  // Flag to track if first lap is complete
bool firstCorrelationDone = false; // Flag to indicate that the first correlation is done
uint16_t referenceData[CORRELATION_WINDOW];   // To store the first 100 samples as reference
uint16_t correlationData[CORRELATION_WINDOW]; // To store the sliding window of current data
uint16_t sliderData[SLIDING_WINDOW]; // To store the sliding window of current data

const int16_t kernel[CONV_KERNEL_SIZE] = {0, 0, 0, -1, -1, 0, 0, 0}; // Convolution kernel, bends, straight sections, ...

// FUNCTIONS

/*
 * Function to calculate cross-correlation
 */
bool corrPerform(const uint16_t *referenceData, const uint16_t *correlationData, uint16_t windowSize)
{
    int32_t maxCorrelation = 0;

    // Compute the correlation for the whole window
    int32_t correlationSum = 0;
    uint16_t i;
//    ble_send("\n\t*** DATA FOR CORRELATION: ***\n ");
//    ble_send("\n\tcurrent\treference\tsum\n");
    for (i = 0; i < windowSize; i++) {
        correlationSum += (int32_t)correlationData[i] * (int32_t)referenceData[i];
//        ble_send("\n\t ");
//        ble_send_int32(correlationData[i]);
//        ble_send("\t");
//        ble_send_int32(referenceData[i]);
//        ble_send("\t\t");
//        ble_send_int32(correlationSum);

    }

    maxCorrelation = correlationSum;
//    ble_send("\nCorrelation max: ");

    // BLE SEND
    ble_send_int32(maxCorrelation);
    ble_send("\n");

    // Check if the correlation is above the CORRELATION_THRESHOLD to detect a new lap
    return maxCorrelation > CORRELATION_THRESHOLD;
}

//
/*
 * Function to collect ADC data and detect laps.
 * This function collect data that are then parsed into the correlation algorithm
 */
bool corrDetectNewLapStart(uint16_t newADCValue)
{
    bool corrResult = false;    //flag for detecting positive correlation results
    // If the reference data is not complete, keep collecting the first samples
    if (!referenceDataDone) {
        // Once we have collected enough samples, store them as reference data
        if (currentDataCounter != CORRELATION_WINDOW) {
            // Store reference samples as referenceData data
            referenceData[currentDataCounter] = newADCValue;
            // Increment the counter
            currentDataCounter++;
        } else {
            referenceDataDone = true; // Mark the first lap as complete
            currentDataCounter = 0; // Reset for collecting new data
            correlationData[currentDataCounter] = newADCValue; // Collect new data
            // Increment the counter
            currentDataCounter++;
        }
    } else { // go here after reference data collected
        if (!firstCorrelationDone) {
            if (currentDataCounter != CORRELATION_WINDOW) {
                correlationData[currentDataCounter] = newADCValue;
                // Increment the counter
                currentDataCounter++;
            } else {
                // run correlation
                if (corrPerform(referenceData, correlationData, CORRELATION_WINDOW)) {
                    // New lap detected, handle the event
                    corrResult = true;
                }
                firstCorrelationDone = true; // Mark the first lap as complete
                currentDataCounter = 0; // Reset for collecting new lap data

                //BLE DEGUB
                #ifdef CORR_BLE_DBG
                ble_send("DBG:\tRef\tCur\n");
                uint8_t ii = 0;
                for (ii = 0; ii < CORRELATION_WINDOW; ii++) {
                    ble_send("\t");
                    ble_send_uint16(referenceData[ii]);
                    ble_send("\t");
                    ble_send_uint16(correlationData[ii]);
                    ble_send("\n");
                }
                #endif
                sliderData[currentDataCounter] = newADCValue; // Collect save new data into slider data and so on
                // Increment the counter
                currentDataCounter++;
                //BLE DEGUB
                #ifdef CORR_BLE_DBG
                ble_send("DBG: First correlation done!\n");
                #endif
            }
        } else { // go here after the first correlation
            // The first correlation is done, continue collecting only sliding window samples
            if (currentDataCounter != SLIDING_WINDOW) {
                // collect the slider data
                sliderData[currentDataCounter] = newADCValue;
                // Increment the counter
                currentDataCounter++;
            } else {
                //BLE DEGUB
                #ifdef CORR_BLE_DBG
                ble_send("DBG: Slider data before sliding current data:\n");
                uint8_t ii = 0;
                for (ii = 0; ii < SLIDING_WINDOW; ii++) {
                    ble_send("\t");
                    ble_send_uint16(sliderData[ii]);
                    ble_send("\t");
                    ble_send("\n");
                }
                #endif
                // Slide the correlationData by SLIDING_WINDOW samples to the left
                uint16_t i;
                for (i = 0; i < CORRELATION_WINDOW - SLIDING_WINDOW; i++) {
                    correlationData[i] = correlationData[i + SLIDING_WINDOW];
                }
                // Add the SLIDING_WINDOW 20 new ADC samples at the end of correlationData
                for (i = 0; i < SLIDING_WINDOW; i++) {
                    // Use the actual ADC value for each new sample
                    correlationData[CORRELATION_WINDOW - SLIDING_WINDOW + i] = sliderData[i];
                }
                //BLE DEGUB
                #ifdef CORR_BLE_DBG
                ble_send("DBG: Slider window full, sliding and running correlation!\n");
                ble_send("DBG: Data after sliding\n");
                ble_send("DBG:\tRef\tCur\tSld\n");
                for (ii = 0; ii < CORRELATION_WINDOW; ii++) {
                    ble_send("\t");
                    ble_send_uint16(referenceData[ii]);
                    ble_send("\t");
                    ble_send_uint16(correlationData[ii]);
                    ble_send("\t");
                    ble_send_uint16(sliderData[ii]);
                    ble_send("\n");
                }
                #endif
                // now correlationData[] has CORRELATION_WINDOW-SLIDING_WINDOW old samples and SLIDING_WINDOW new samples
                // run correlation
                if (corrPerform(referenceData, correlationData, CORRELATION_WINDOW)) {
                    // New lap detected, handle the event
                    corrResult = true;
                }
                currentDataCounter = 0; // Reset for collecting new lap data
                sliderData[currentDataCounter] = newADCValue; // Collect new data
                // Increment the counter
                currentDataCounter++;
            }
        }
    }
    return corrResult;   // Return the final result at the end
}

/*
 * Function to clean correlation buffers and variables in case of a need of starting from scratch
 */
void corrClearBuffers(void)
{
    currentDataCounter = 0;  // Track current number of collected samples
    referenceDataDone = false;  // Flag to track if first lap is complete
    firstCorrelationDone = false; // Flag to indicate that the first correlation is done

    // clear arrays
    memset(referenceData, 0, sizeof(referenceData));
    memset(referenceData, 0, sizeof(referenceData));
    memset(sliderData, 0, sizeof(sliderData));
}

/*
 * Function to perform convolution on the ADC data
 */
int16_t convPerform(uint16_t* data, uint32_t length, uint32_t index) {
    int16_t conv_result = 0;
    uint8_t i = 0;

    // Apply kernel only if there's enough data to compute it
    if (index >= CONV_KERNEL_SIZE / 2 && index < length - CONV_KERNEL_SIZE / 2) {
        for (i = 0; i < CONV_KERNEL_SIZE; i++) {
            conv_result += data[index - CONV_KERNEL_SIZE / 2 + i] * kernel[i];
        }
    }
    return conv_result;
}

// Function to determine if the car is in a bend or straight section
void convAnalyzeKernel(uint16_t* data, uint32_t length) {
    uint32_t i = 0;
    uint8_t bend_count = 0;
    int16_t conv_value;
    uint8_t length_temp = 20;

    for (i = 0; i < length; i++) {
        conv_value = convPerform(data, length, i);
//        ble_send("FINAL conv_value: ");
        ble_send_int16(conv_value);
        ble_send("\n");
//        ble_send("\n");

        // Check if conv_value exceeds CONV_THRESHOLD consistently
        if (conv_value > CONV_THRESHOLD) {
            bend_count++;
            ble_send("\nfind a HIGHT value.\n ");
            if (bend_count >= CONV_DEBOUNCE_COUNT) {
//                state_transition(STATE_SLOW_DOWN);  // Example state transition for bends
                bend_count = 0; // Reset after transition
            }
        } else {
            bend_count = 0;

//            state_transition(STATE_NORMAL_SPEED);  // Straight path state transition
        }

        if (conv_value < CONV_THRESHOLD) {
                    ble_send("\nfind a low value - two low values.\n ");
                }
    }

}
