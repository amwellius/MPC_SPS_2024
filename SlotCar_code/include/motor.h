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
#define H_BRAKE_PORT     P8OUT  // P8.5/TA1.0
#define H_STATUS_PORT    P8IN   // P8.4/TA0.4  INPUT PORT
#define H_DIRECTION_PORT P8OUT  // P8.2/TA0.2
#define H_IN_PORT        P8OUT  // P8.3/TA0.3

// functions prototypes
#define H_BRAKE_PIN     0x20    // pin driven low by the H_BRIDGE in case of Vfault
#define H_STATUS_PIN    0x10    // status pin = gives fault info
#define H_DIRECTION_PIN 0x04
#define H_IN_PIN        0x08

// H_Bridge control
#define H_IN_h          (H_IN_PORT |= H_IN_PIN)     // sets pin to high
#define H_IN_l          (H_IN_PORT &= ~H_IN_PIN)    // sets pin to low
#define H_BRAKE_h       (H_BRAKE_PORT |= H_BRAKE_PIN)
#define H_BRAKE_l       (H_BRAKE_PORT &= ~H_BRAKE_PIN)
// These below pins can be operated by PWM control. PWM 50%+ changes forward speed, 50%- is reverse speed
#define H_DIRECTION_l   (H_DIRECTION_PORT |= H_DIRECTION_PIN)   // reversed h and l (dont know why but it works this way)
#define H_DIRECTION_h   (H_DIRECTION_PORT &= ~H_DIRECTION_PIN)  // reversed h and l (dont know why but it works this way)

// variables


// FUNCTIONS
void motor_init(void);
void motor_forward(void);
void motor_reverse(void);
void motor_brake(void);
void motor_idle(void);


#endif /* MOTOR_H_ */
