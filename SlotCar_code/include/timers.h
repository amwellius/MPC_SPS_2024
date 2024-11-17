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

// DEFINITIONS
#define one_ms              1   // 1 =  Approximately 1 milisecond
#define thirdy_one_msec     1   // 1  = Approximately 31.75 miliseconds
#define sixtytwo_msec       2   // 2  = Approximately 62.5 miliseconds
#define half_sec            16  // 16 = Approximately 0.5 seconds
#define one_sec             32  // 32 = Approximately 1 seconds
#define max_variable_delays 10  // Define maximum delays for the variable delay function

#define BRAKE_RELEASE_LEDS_MS 400   // Define time in ms to release LEDs after releasing brakes

// VARIABLES
extern volatile unsigned char flag_1ms;          // Declare flag_1ms as extern
extern volatile unsigned char flag_31ms;         // Declare flag_31.75ms as extern
extern volatile unsigned char flag_62ms;         // Declare flag_62.5ms as extern
extern volatile unsigned char flag_500ms;        // Declare flag_500ms as extern
extern volatile unsigned char flag_1000ms;       // Declare flag_1000ms as extern
bool static brake_release_counter_start;         // Declare flag to restart brake LEDs counter

// FUNCTIONS
void initClockTo16MHz(void);                      // main clock 16MHz
void init_timerA0(uint16_t duty_cycle);           // timer A, instance 0 for PWM signal generation
void init_timerA1(void);                          // timer A, instance 1 for precise timed interrupts ~1ms+
void init_timerB0(void);                          // timer B, instance 0 for precise timed interrupts ~31ms+

bool variable_delay_ms(uint8_t index, uint16_t delay_ms); // variable delay in ms; index of delay; delay in ms

#endif /* LED_H_ */
