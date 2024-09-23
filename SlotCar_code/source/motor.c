/*
 * motor.c
 *
 *  Created on: 29. 9. 2020
 *      Author: dosedel
 */

#include "include/motor.h"

void motor_init() {
    P8DIR |= 0x0C; // P8.2 and P8.3 as output
}
