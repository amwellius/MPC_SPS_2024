/*
 * motor.c
 *
 *  Created on: 23/September/2024
 *      Author: xkosik09
 */

// INCLUDES
#include "include/motor.h"
#include "include/timers.h"

// FUNCTIONS
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

/* Function for using the PWM signal for controlling the motor.
 * PWM levels and numbers in motor.h file
 */
void motor_pwm(pwm_level_t level)
{
    uint16_t duty_cycle = level; // Map level to duty cycle
    init_timerA0(duty_cycle);
    H_IN_h;
    H_BRAKE_l;
}
