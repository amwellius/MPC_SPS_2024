/*
 * timers.h
 *
 *  Created on: 05/October/2024
 *      Author: xkosik09
 */

#ifndef timers_H_
#define timers_H_

// includes
#include <msp430.h>

// Variables
extern volatile unsigned int overflow_count1;    // Declare overflow_count1 as extern
extern volatile unsigned int overflow_count2;    // Declare overflow_count2 as extern
extern volatile unsigned char flag_500ms;        // Declare flag_500ms as extern
extern volatile unsigned char flag_1000ms;       // Declare flag_1000ms as extern

// FUNCTIONS
void initClockTo16MHz(void);                                        // main clock 16MHz
void init_timerA0(unsigned int duty_cycle);                         // timer A, instance 0 for PWM signal generation
void init_timerB0(void);                                            // timer B, instance 0 for precise timed interrupts


#endif /* LED_H_ */
