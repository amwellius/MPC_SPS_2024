/*
 * timers.h
 *
 *  Created on: 05/October/2024
 *      Author: xkosik09
 */

#ifndef timers_H_
#define timers_H_

// INCLUDES
#include <msp430.h>
#include "stdint.h"
#include <stdbool.h>

// VARIABLES
//extern volatile uint8_t overflow_count1;         // Declare overflow_count1 as extern
//extern volatile uint8_t overflow_count2;         // Declare overflow_count2 as extern
//extern volatile uint8_t overflow_count3;         // Declare overflow_count3 as extern
//extern volatile uint8_t overflow_count4;         // Declare overflow_count4 as extern
//extern volatile uint8_t overflow_count5;         // Declare overflow_count5 as extern
//extern volatile uint16_t variable_ms;            // Declare variable_ms
extern volatile unsigned char flag_1ms;          // Declare flag_1ms as extern
extern volatile unsigned char flag_31ms;         // Declare flag_31.75ms as extern
extern volatile unsigned char flag_62ms;         // Declare flag_62.5ms as extern
extern volatile unsigned char flag_500ms;        // Declare flag_500ms as extern
extern volatile unsigned char flag_1000ms;       // Declare flag_1000ms as extern
extern volatile unsigned char flag_variable_ms;  // Declare flag_variable_ms as extern

//extern volatile uint16_t overflow_count6;

// FUNCTIONS
void initClockTo16MHz(void);                      // main clock 16MHz
void init_timerA0(uint16_t duty_cycle);           // timer A, instance 0 for PWM signal generation
void init_timerA1(void);                          // timer A, instance 1 for precise timed interrupts ~1ms+
void init_timerB0(void);                          // timer B, instance 0 for precise timed interrupts ~31ms+

bool variable_delay_ms(uint8_t index, uint16_t delay_ms);

#endif /* LED_H_ */
