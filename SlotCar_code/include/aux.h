/*
 * aux.h
 *
 *  Created on: 22/October/2024
 *      Author: xkosik09
 */

#ifndef AUX_H_
#define AUX_H_

// INCLUDES
#include <msp430.h>

// DEFINITIONS
#define axis_enable_z // set z y x for axis to be enabled for ADC motor control

// VARIABLES

// FUNCTIONS
void car_control_simple(void);      // simple motor PWM ADC data controlling

#endif /* AUX_H_ */
