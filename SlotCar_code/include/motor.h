/*
 * motor.h
 *
 *  Created on: 29. 9. 2020
 *      Author: dosedel
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <msp430.h>
// macros definition
#define H_BREAK_PORT     P8OUT
#define H_STATUS_PORT    P8OUT
#define H_DIRECTION_PORT P8OUT
#define H_IN_PORT        P8OUT // P8.3/TA0.3

// functions prototypes
#define H_BREAK_PIN
#define H_STATUS_PIN
#define H_DIRECTION_PIN 0x04
#define H_IN_PIN        0x08

// Motor control
#define MOTOR_ON() (H_IN_PORT |= H_IN_PIN)
#define MOTOR_OFF() (H_IN_PORT &= ~H_IN_PIN)

#define MOTOR_FW() (H_DIRECTION_PORT |= H_DIRECTION_PIN)
#define MOTOR_RV() (H_DIRECTION_PORT &= ~H_DIRECTION_PIN)

// FUNCTIONS
void motor_init();

// variables

#endif /* MOTOR_H_ */
