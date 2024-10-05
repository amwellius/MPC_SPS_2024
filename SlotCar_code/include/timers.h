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
extern volatile unsigned int overflow_count;    // Declare overflow_count as extern
extern volatile unsigned char motor_flag;       // Declare motor_flag as extern

// FUNCTIONS
void initClockTo16MHz(void);        // main clock 16MHz
void test_timer(void);              // test timers functions for


#endif /* LED_H_ */
