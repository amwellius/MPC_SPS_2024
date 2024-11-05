/*
 * ADC.c
 *
 *  Created on: 24. 9. 2020
 *      Author: dosedel
 * Modified on: 07/October/2024
 *      Author: xkosik09
 */

// INCLUDES
#include "include/ADC.h"
#include <msp430.h>

// VARIABLES
volatile uint16_t ADC_raw_results[5];  // Define the array
volatile uint16_t ADC_filtered_results[5];

// FUNCTIONS
void ADC_init(void)
{
    P6SEL |= 0xF8;      // P6.3 - P6.7 as ADC

    ADC12CTL0 = ADC12ON+ADC12MSC+ADC12SHT0_2; // Turn on ADC12, set sampling time, fs is roughly 20kHz
    ADC12CTL1 = ADC12SHP+ADC12CONSEQ_1;       // Use sampling timer, single sequence
    ADC12MCTL3 = ADC12INCH_3;                 // ref+=AVcc, channel = A3
    ADC12MCTL4 = ADC12INCH_4;                 // ref+=AVcc, channel = A4
    ADC12MCTL5 = ADC12INCH_5;                 // ref+=AVcc, channel = A5
    ADC12MCTL6 = ADC12INCH_6;                 // ref+=AVcc, channel = A6
    ADC12MCTL7 = ADC12INCH_7+ADC12EOS;        // ref+=AVcc, channel = A7, end seq.
    ADC12IE = ADC12IE7;                       // Enable ADC12IFG.7
    ADC12CTL0 |= ADC12ENC;                    // Enable conversions
}


void ADC_start(void)
{
    /*
     * Once the ADC is started, it samples all the time.
     * It saves the values into array results[].
     *
     */
    ADC12CTL0 |= ADC12SC;                   // Start convn - software trigger
}

uint16_t ADC_get_result(uint8_t index)
{
#ifdef filter_ON
    return ADC_filtered_results[index];
#else
    return ADC_raw_results[index];
#endif
}

// Function to add a new sample and compute the moving average
int32_t moving_average(void)
{
    volatile uint32_t new_sample = ADC_raw_results[4];
    static int32_t samples[WINDOW_SIZE] = {0};  // Buffer for storing samples
    static uint16_t index = 0;                   // Current index for the buffer
    static uint32_t sum = 0;                     // Sum of the samples

    // Remove the oldest sample from the sum
    sum -= samples[index];

    // Add the new sample to the buffer and the sum
    samples[index] = new_sample;
    sum += new_sample;

    // Update the index (wrap around)
    index = (index + 1) % WINDOW_SIZE;

    // Return the average
    return sum / WINDOW_SIZE;
}

// **************************************INTERUPTS************************************** //
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6: break;                           // Vector  6:  ADC12IFG0
  case  8: break;                           // Vector  8:  ADC12IFG1
  case 10: break;                           // Vector 10:  ADC12IFG2
  case 12: break;                           // Vector 12:  ADC12IFG3
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20:                            // Vector 20:  ADC12IFG7
//      LED_FL_toggle();
      ADC12CTL0 &=~ADC12SC;                // For sequence-of-Channels mode, ADC12SC must be cleared by software after each sequence to trigger another sequence
      ADC_raw_results[0] = ADC12MEM3;      // Move results, IFG is cleared
      ADC_raw_results[1] = ADC12MEM4;      // Move results, IFG is cleared
      ADC_raw_results[2] = ADC12MEM5;      // Move results, IFG is cleared // X (up / down)
      ADC_raw_results[3] = ADC12MEM6;      // Move results, IFG is cleared // Y (forward-backward)
      ADC_raw_results[4] = ADC12MEM7;      // Move results, IFG is cleared // Z (left-right)
//      LED_FL_OFF();
      ADC12CTL0 |= ADC12SC;                   // Start convn - software trigger
    #ifdef filter_ON
      ADC_filtered_results[4] = moving_average();
    #endif
      __bic_SR_register_on_exit(LPM0_bits);   // exit low power mode if entered
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}
