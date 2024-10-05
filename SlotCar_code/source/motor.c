/*
 * motor.c
 *
 *  Created on: 29. 9. 2020
 *      Author: dosedel
 */

#include "include/motor.h"

void motor_init(void) {
    P8DIR |=    0x2C; // P8.2, P8.3 and P8.5 as outputs
    P8DIR &= ~  0x10; //P8.4 as input
    motor_idle();
}

// function for motor control forward
void motor_forward(void) {
    H_IN_h;
    H_DIRECTION_h;
    H_BRAKE_l;
}

// function for motor control reverse
void motor_reverse(void) {
    H_IN_h;
    H_DIRECTION_l;
    H_BRAKE_l;
}

// function for motor control brake
void motor_brake(void) {
    H_IN_l;
    H_DIRECTION_l;
    H_BRAKE_h;
}

// function for motor control idle. This means the motor does not have power
// the motor doesn't do anything here
void motor_idle(void) {
    H_IN_l;
    H_DIRECTION_l;
    H_BRAKE_l;
}
