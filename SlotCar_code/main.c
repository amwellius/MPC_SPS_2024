/**
 * main.c
 */

// INCLUDES
#include <msp430.h>
#include "include/timers.h"
#include "include/SPI.h"
#include "include/I2C.h"
#include "include/LED.h"
#include "include/ADC.h"
#include "include/L3GD20H.h"
#include "include/ADXL343.h"
#include "include/motor.h"
#include "include/UART.h"
#include "include/aux.h"
#include "include/flash_memory.h"
#include "include/data_temp_storage.h"

// VARIABLES


//// tests //////
//#define KERNEL_SIZE 8  // Define based on expected pattern size
//#define THRESHOLD_conv -2000 // Example threshold; adjust based on testing
//#define DEBOUNCE_COUNT 3  // Number of consistent readings before state change

// Define your convolution kernel
//const int16_t kernel[KERNEL_SIZE] = {0, 0, 0, -1, -1, 0, 0, 0}; // Example kernel for bends



// Function to perform convolution on the ADC data
//int16_t perform_convolution(uint16_t* data, uint32_t length, uint32_t index) {
//    int16_t conv_result = 0;
//    uint8_t i = 0;
//
//    // Apply kernel only if there's enough data to compute it
//    if (index >= KERNEL_SIZE / 2 && index < length - KERNEL_SIZE / 2) {
//        for (i = 0; i < KERNEL_SIZE; i++) {
//            conv_result += data[index - KERNEL_SIZE / 2 + i] * kernel[i];
//        }
//    }
//    return conv_result;
//}



// Function to determine if the car is in a bend or straight section
//void analyze_track_section(uint16_t* data, uint32_t length) {
//    uint32_t i = 0;
//    uint8_t bend_count = 0;
//    int16_t conv_value;
//    uint8_t length_temp = 20;
//
//    for (i = 0; i < length; i++) {
//        conv_value = perform_convolution(data, length, i);
////        ble_send("FINAL conv_value: ");
//        ble_send_int16(conv_value);
//        ble_send("\n");
////        ble_send("\n");
//
//        // Check if conv_value exceeds threshold consistently
//        if (conv_value > THRESHOLD_conv) {
//            bend_count++;
//            ble_send("\nfind a HIGHT value.\n ");
//            if (bend_count >= DEBOUNCE_COUNT) {
////                state_transition(STATE_SLOW_DOWN);  // Example state transition for bends
//                bend_count = 0; // Reset after transition
//            }
//        } else {
//            bend_count = 0;
//
////            state_transition(STATE_NORMAL_SPEED);  // Straight path state transition
//        }
//
//        if (conv_value < THRESHOLD_conv) {
//                    ble_send("\nfind a low value - two low values.\n ");
//                }
//    }
//
//}
//// tests //////

// cross-correlation example
//// tests //////
//#include <stdint.h>
//#include <stdbool.h>
//
//#define REFERENCE_LAP_LENGTH 100  // Adjust based on actual reference lap length
//#define THRESHOLD 50000           // Set based on expected peak magnitude
//
//// Function to calculate cross-correlation and detect a new lap
//bool detectNewLapStart(const uint16_t *referenceLap, const uint16_t *correlationData, uint16_t currentLapLength) {
//    int32_t maxCorrelation = 0;
//    uint16_t i, j;
//
//    // Slide the reference lap over the current data
//    for (i = 0; i <= currentLapLength - REFERENCE_LAP_LENGTH; i++) {
//        int32_t correlationSum = 0;
//
//        // Compute cross-correlation for this window
//        for (j = 0; j < REFERENCE_LAP_LENGTH; j++) {
//            correlationSum += correlationData[i + j] * referenceLap[j];
//        }
//
//        // Check if this is the highest correlation found
//        if (correlationSum > maxCorrelation && correlationSum > THRESHOLD) {
//            maxCorrelation = correlationSum;
//        }
//    }
//
//    // If max correlation exceeds threshold, assume new lap started
//    return maxCorrelation > THRESHOLD;
//}
//// tests //////

// cross-correlation example
//// tests //////
#include <stdint.h>
#include <stdbool.h>

#define WINDOW_SIZE_corr 5   // Number of samples for reference lap
#define SLIDING_WINDOW 2     // Number of samples to slide in the window
#define THRESHOLD 19334000 //211846000   // Threshold for cross-correlation to detect a new lap

uint16_t referenceLap[WINDOW_SIZE_corr];   // To store the first 100 samples as reference
uint16_t correlationData[WINDOW_SIZE_corr]; // To store the sliding window of current data
uint16_t sliderData[SLIDING_WINDOW]; // To store the sliding window of current data

uint16_t currentLapLength = 0;  // Track current number of collected samples
bool reference_data_complete = false;  // Flag to track if first lap is complete
bool firstCorrelationDone = false; // Flag to indicate that the first correlation is done

// Function to calculate cross-correlation
bool detectNewLapStart(const uint16_t *referenceLap, const uint16_t *correlationData, uint16_t windowSize)
{
    int32_t maxCorrelation = 0;

    // Compute the correlation for the whole window
    int32_t correlationSum = 0;
    uint16_t i;
//    ble_send("\n\t*** DATA FOR CORRELATION: ***\n ");
//    ble_send("\n\tcurrent\treference\tsum\n");
    for (i = 0; i < windowSize; i++) {
        correlationSum += (int32_t)correlationData[i] * (int32_t)referenceLap[i];
//        ble_send("\n\t ");
//        ble_send_int32(correlationData[i]);
//        ble_send("\t");
//        ble_send_int32(referenceLap[i]);
//        ble_send("\t\t");
//        ble_send_int32(correlationSum);

    }

    maxCorrelation = correlationSum;
//    ble_send("\nCorrelation max: ");

//    ble_send_int32(maxCorrelation);
//    ble_send("\n");

    // Check if the correlation is above the threshold to detect a new lap
    return maxCorrelation > THRESHOLD;
}

// Function to calculate sliding cross-correlation
//bool detectNewLapStart(const uint16_t *referenceLap, const uint16_t *correlationData, uint16_t windowSize) {
//    int32_t maxCorrelation = 0;
//    int32_t correlationSum = 0;
//    uint16_t shift, i;
//
//    // Loop over each shift of correlationData against referenceLap
//    for (shift = 0; shift <= (windowSize - 1); shift++) {
//        correlationSum = 0;
//
//        // Compute the correlation for this shift (dot product of the shifted windows)
//        for (i = 0; i < windowSize - shift; i++) {
//            correlationSum += (int32_t)correlationData[i + shift] * (int32_t)referenceLap[i];
//        }
//
//        // Update maxCorrelation if this shift produces a higher correlation
//        if (correlationSum > maxCorrelation) {
//            maxCorrelation = correlationSum;
//        }
//    }
//        ble_send_int32(maxCorrelation);
//        ble_send("\n");
//    // Check if the maxCorrelation exceeds the threshold to detect a new lap
//    return maxCorrelation > THRESHOLD;
//}


// Function to collect ADC data and detect laps
void collectADCData(uint16_t newADCValue)
{
    // If the reference data is not complete, keep collecting the first 100 samples
    if (!reference_data_complete) {
//        correlationData[currentLapLength] = newADCValue;
//        currentLapLength++;

        // Once we have collected 100 samples, store them as reference lap data
        if (currentLapLength != WINDOW_SIZE_corr)
        {
            // Store reference samples as referenceLap data
            referenceLap[currentLapLength] = newADCValue;
            // Increment the counter
            currentLapLength++;
        }
        else
        {
            reference_data_complete = true;  // Mark the first lap as complete
            currentLapLength = 0;  // Reset for collecting new lap data
            correlationData[currentLapLength] = newADCValue; // Collect new data
            // Increment the counter
            currentLapLength++;
        }
    }
    else // go here after reference data collected
    {
        if (!firstCorrelationDone) {
            if (currentLapLength != WINDOW_SIZE_corr) {
                correlationData[currentLapLength] = newADCValue;
                // Increment the counter
                currentLapLength++;
            }
            else
            {
                // *** run correlation HERE ***
                if (detectNewLapStart(referenceLap, correlationData, WINDOW_SIZE_corr)) {
                    // New lap detected, handle the event (reset FSM, etc.)
//                    currentLapLength = 0;  // Reset for new lap
//                    ble_send_int32(20000000);
                    //                    ble_send("\n");
                }
                firstCorrelationDone = true;  // Mark the first lap as complete
                currentLapLength = 0;  // Reset for collecting new lap data

                //BLE DEGUB
                ble_send("DBG:\tRef\tCur\n");
                uint8_t ii = 0;
                for (ii=0;ii<WINDOW_SIZE_corr; ii++){
                    ble_send("\t");
                    ble_send_uint16(referenceLap[ii]);
                    ble_send("\t");
                    ble_send_uint16(correlationData[ii]);
                    ble_send("\n");
                }

                sliderData[currentLapLength] = newADCValue; // Collect save new data into slider data and so on
                // Increment the counter
                currentLapLength++;
                //BLE DEGUB
                ble_send("DBG: First correlation done!\n");
            }
        }
        else // go here after the first correlation
        {
            // The first correlation is done, continue collecting only sliding window samples
            if (currentLapLength != SLIDING_WINDOW) {
                // collect the slider data
                sliderData[currentLapLength] = newADCValue;
                // Increment the counter
                currentLapLength++;
                }
            else {
                //BLE DEGUB
                ble_send("DBG: Slider data before sliding current data:\n");
                uint8_t ii = 0;
                for (ii=0;ii<SLIDING_WINDOW; ii++){
                    ble_send("\t");
                    ble_send_uint16(sliderData[ii]);
                    ble_send("\t");
                    ble_send("\n");
                }

                // Slide the correlationData by 20 samples to the left
                uint16_t i;
                for (i = 0; i < WINDOW_SIZE_corr - SLIDING_WINDOW; i++) {
                    correlationData[i] = correlationData[i + SLIDING_WINDOW];
                }

                // Add the next 20 new ADC samples at the end of correlationData
                for (i = 0; i < SLIDING_WINDOW; i++) {
                    // Use the actual ADC value for each new sample
                    correlationData[WINDOW_SIZE_corr - SLIDING_WINDOW + i] = sliderData[i];
                }
                //BLE DEGUB
                ble_send("DBG: Slider window full, sliding and running correlation!\n");
                ble_send("DBG: Data after sliding\n");
                ble_send("DBG:\tRef\tCur\tSld\n");
                for (ii=0;ii<WINDOW_SIZE_corr; ii++){
                    ble_send("\t");
                    ble_send_uint16(referenceLap[ii]);
                    ble_send("\t");
                    ble_send_uint16(correlationData[ii]);
                    ble_send("\t");
                    ble_send_uint16(sliderData[ii]);
                    ble_send("\n");
                }

                // now correlationData[] has 80 old samples and 20 new samples
                // *** run correlation HERE ***
                if (detectNewLapStart(referenceLap, correlationData, WINDOW_SIZE_corr)) {
                    // New lap detected, handle the event (reset FSM, etc.)
//                    currentLapLength = 0;  // Reset for new lap
//                    ble_send_int32(20000000);
                    //                    ble_send("\n");
                }
                currentLapLength = 0;  // Reset for collecting new lap data
                sliderData[currentLapLength] = newADCValue; // Collect new data
                // Increment the counter
                currentLapLength++;
            }
            }
        }
}


//
//        if (currentLapLength != SLIDING_WINDOW)
//        {
//            // Store current sample as current data
//            correlationData[currentLapLength] = newADCValue;
//            // Increment the counter
//            currentLapLength++;
//        }

//
//
//
//
//        // Once the reference data is collected, start collecting data for current window
//        if (currentLapLength < WINDOW_SIZE_corr) {
//            // Keep collecting samples until we have 100 in correlationData
//            correlationData[currentLapLength] = newADCValue;
//            currentLapLength++;
//
////            //Once we have the second set of 100 samples, run the first correlation
////            if (currentLapLength == WINDOW_SIZE_corr) {
////                if (detectNewLapStart(referenceLap, correlationData, WINDOW_SIZE_corr)) {
////                    // New lap detected, handle the event (reset FSM, etc.)
////                    firstCorrelationDone = true;  // Mark the first correlation as done
////                    currentLapLength = 0;  // Reset for new lap
////                }
////            }
//        } else {
//            // After the first correlation, we replace the oldest 20 values with the new 20 samples
////            if (firstCorrelationDone) {
//                // Slide the correlationData by 20 samples to the left
//                uint16_t i;
//                for (i = 0; i < WINDOW_SIZE_corr - SLIDING_WINDOW; i++) {
//                    correlationData[i] = correlationData[i + SLIDING_WINDOW];
//                }
//
//                // Add the next 20 new ADC samples at the end of correlationData
//                for (i = 0; i < SLIDING_WINDOW; i++) {
//                    correlationData[WINDOW_SIZE - SLIDING_WINDOW + i] = newADCValue; // Use the actual ADC value for each new sample
//                }}
//
//                // Run the correlation after every 20 new samples are added
//                if (detectNewLapStart(referenceLap, correlationData, WINDOW_SIZE_corr)) {
//                    // New lap detected, handle the event (reset FSM, etc.)
//                    currentLapLength = 0;  // Reset for new lap
//                    ble_send_int32(20000000);
//                    ble_send("\n");
//                }
//                //temp term for saving data
//                correlationData[currentLapLength] = newADCValue;
//                currentLapLength = 0;  // Reset for new lap++;
//            }
////        ble_send("currentLapLength: ");
////        ble_send_uint16(currentLapLength);
////        ble_send("\n");
//        }
//    }
//}
//// tests //////





// MAIN LOOP
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    initClockTo16MHz();         // initialize clock timer 16 MHz
    __bis_SR_register(GIE);     // Enable global interrupts
    _BIS_SR(GIE);               // some interrupts as well
    __enable_interrupt();


    LED_init();
    init_timerB0();
    init_timerA1();
//    motor_init();
//    ADC_init();
//    ADC_start();
    UART_init();


    //// tests //////
//    uint16_t i = 0;
//    ble_send("\n \t *** STARTING STATE MACHINE ***\n\n");
//    state_machine_init();  // Initialize the state machine
    //// tests //////

//    while (1)
//    {
        //// tests //////
//        car_control_FSM();
        //// tests //////

//        if (variable_delay_ms(1, 1000)) {
////            LED_FR_toggle(); // Example task
//        }
//
//
//        if (variable_delay_ms(2, 62)) {
//
//            if (i<SAMPLE_COUNT)
//            {
////                ble_send_uint16(adc_data[i]);
////                ble_send("\n");
//                i++;
//            }
//
//            switch(adc_data[i])
//            {
//            case 0 ... 1959:    // momentum vector RIGHT, RIGHT LED ON
//                LED_RR_ON();
//                LED_RL_OFF();
//                motor_pwm(PWM_LEVEL_4);
//                break;
//            case 1970 ... 4095: // momentum vector LEFT, LEFT LED ON
//                LED_RR_OFF();
//                LED_RL_ON();
//                motor_pwm(PWM_LEVEL_4);
//                break;
//            default:
//                LED_RL_OFF();
//                LED_RR_OFF();
//                motor_pwm(PWM_LEVEL_5);
//                break;
//            }
//
//            if (i==SAMPLE_COUNT)
//            {
//                i = 0;
//            }
//            LED_FL_toggle();
//
//        }

//    }

//    car_control_FSM();


    //// tests //////
//    ble_send("\nKERNEL: 1 0 -1 0 0 1\n");
//    analyze_track_section(adc_data,SAMPLE_COUNT);
//    uint16_t temp_adc_data;
    uint16_t ii = 0;
    while(1){

//        if (variable_delay_ms(1, 400)) {
//            // Perform another task every xxx ms
////            analyze_track_section(adc_data,SAMPLE_COUNT);
//            LED_FR_toggle(); // Example task
//        }

//        for (ii=0; ii<SAMPLE_COUNT; ii++)
//        {
//            collectADCData(adc_data[ii]);
////            ble_send("\nfind a match in the data!\n");
//        }

        if (variable_delay_ms(5, 10)) {
        // Perform task every xxx ms
            LED_FR_toggle(); // Example task
            if (ii<SAMPLE_COUNT) {
                collectADCData(adc_data[ii]);
//                ble_send("loops: ");
//                ble_send_uint16(ii);
//                ble_send("\n");
                ii++;
            }
            if (ii == SAMPLE_COUNT)
            {
                ble_send_int32(18500000);
                ble_send("\n");
                ii = 0;
//                ble_send("\na loop thru the data samples!\n");
            }
        }

        if (variable_delay_ms(6, 500)) {
        LED_FL_toggle(); // Example task
        }





    }

    //// tests //////









    // infinite loop
    while (1)
    {
        car_control_simple();
//        car_control_FSM();

        if (variable_delay_ms(0, 100)) {
            // Perform task every xxx ms
            LED_FL_toggle(); // Example task
        }

        if (variable_delay_ms(1, 300)) {
            // Perform another task every xxx ms
            LED_FR_toggle(); // Example task
        }



        // 1ms interrupt
        if (flag_1ms) {               // Check if flag is set
//            temp_data = moving_average();
            flag_1ms = 0;             // Clear the flag
        }

        // 31.75ms interrupt
        if (flag_31ms) {               // Check if flag is set
//            LED_FL_toggle();
            flag_31ms = 0;             // Clear the flag
        }

        // 500ms interrupt
        if (flag_500ms) {               // Check if flag is set
//            ble_send("Every 500ms\n");
//            ble_send("\n");
            flag_500ms = 0;             // Clear the flag
        }

        // 1000ms interrupt
        if (flag_1000ms) {
            if (povol_TX == 1)
            {
            }

            flag_1000ms = 0;
        }





    }
}


//**********************************************************************************************




