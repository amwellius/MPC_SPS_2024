/*
 * motor.h
 *
 *  Created on: 23/September/2024
 *      Author: xkosik09
 */

#ifndef MOTOR_H_
#define MOTOR_H_

// INCLUDES
#include <msp430.h>
#include "stdint.h"
#include <stdbool.h>

// DEFINITIONS
#define H_BRAKE_PORT     P8OUT  // P8.5/TA1.0
#define H_STATUS_PORT    P8IN   // P8.4/TA0.4  INPUT PORT
#define H_DIRECTION_PORT P8OUT  // P8.2/TA0.2
#define H_IN_PORT        P8OUT  // P8.3/TA0.3

// PINS
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

// VARIABLES
// Define levels for motor PWM control
typedef enum {
    PWM_LEVEL_1  = 300, // 25%      // 0.25 m/s
    PWM_LEVEL_2  = 280, // 30%      // 0.28 m/s
    PWM_LEVEL_3  = 240, // 40%      // 0.60 m/s (updated 25/Nov/2024) - please double-check again!
    PWM_LEVEL_4  = 200, // 50%      // 0.75 m/s
    PWM_LEVEL_5  = 160, // 60%      // 1.00 m/s
    PWM_LEVEL_6  = 120, // 70%      // 1.20 m/s
    PWM_LEVEL_7  = 100, // 75%      // 1.25 m/s
    PWM_LEVEL_8  = 80,  // 80%      // 1.40 m/s
    PWM_LEVEL_9  = 40,  // 90%      // 1.80 m/s
    PWM_LEVEL_10 = 000  // 100%     // 2.90 m/s
} pwm_level_t;

// Define levels for braking
typedef enum {
    BRAKE_LEVEL_1    = 50,     // value in ms
    BRAKE_LEVEL_2    = 60,
    BRAKE_LEVEL_3    = 100,
    BRAKE_LEVEL_4    = 130,
    BRAKE_LEVEL_5    = 160,
    BRAKE_LEVEL_6    = 190,
    BRAKE_LEVEL_7    = 230,
    BRAKE_LEVEL_8    = 400,
    BRAKE_LEVEL_9    = 450,
    BRAKE_LEVEL_10   = 500,
    BRAKE_LEVEL_INF  = 0,      // value used to never release while called flag_brakes_applied
}brake_level_t;

// VARIABLES
extern volatile uint16_t brakes_strength;
extern volatile bool flag_brakes_applied;


// FUNCTIONS
void motor_init(void);
void motor_forward(void);
void motor_pwm(pwm_level_t level);
void motor_reverse(void);
void motor_brake(brake_level_t brakes_strength);
void motor_idle(void);


#endif /* MOTOR_H_ */
