/*
 * LED.h
 *
 *  Created on: 24. 9. 2020
 *      Author: dosedel
 * Modified on: 23/September/2024
 *      Author: xkosik09
 */

#ifndef LED_H_
#define LED_H_

// INCLUDES
#include "stdint.h"
#include <msp430.h>
#include <stdbool.h>

// DEFINITIONS
// LED ports definition
#define LED_FL_PORT  P1OUT
#define LED_FR_PORT  P1OUT
#define LED_RL_PORT  P3OUT
#define LED_RR_PORT  P4OUT
// LED pins definition
#define LED_FL_PIN  0x08
#define LED_FR_PIN  0x04
#define LED_RL_PIN  0x40
#define LED_RR_PIN  0x04

/* LEDs control:
 * F = front, R = rear
 * L = left, R = right
 */

#define LED_FL_ON()     (LED_FL_PORT |= LED_FL_PIN)
#define LED_FL_toggle() (LED_FL_PORT ^= LED_FL_PIN)
#define LED_FL_OFF()    (LED_FL_PORT &= ~LED_FL_PIN)

#define LED_FR_ON()     (LED_FR_PORT |= LED_FR_PIN)
#define LED_FR_toggle() (LED_FR_PORT ^= LED_FR_PIN)
#define LED_FR_OFF()    (LED_FR_PORT &= ~LED_FR_PIN)

#define LED_RL_ON()     (LED_RL_PORT |= LED_RL_PIN)
#define LED_RL_toggle() (LED_RL_PORT ^= LED_RL_PIN)
#define LED_RL_OFF()    (LED_RL_PORT &= ~LED_RL_PIN)

#define LED_RR_ON()     (LED_RR_PORT |= LED_RR_PIN)
#define LED_RR_toggle() (LED_RR_PORT ^= LED_RR_PIN)
#define LED_RR_OFF()    (LED_RR_PORT &= ~LED_RR_PIN)

// FUNCTIONS
void LED_init(void);
void led_flash_all(uint8_t const n); // number of flashes
void led_brake(void);

//////
extern volatile unsigned char flag_brakes_applied;


#endif /* LED_H_ */
