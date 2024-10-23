/*
 * UART.h
 *
 *
 *  Created on: 28. 9. 2021
 *      Author: 
 */

#ifndef UART_H_
#define UART_H_

// INCLUDES
#include "stdint.h"
#include <msp430.h>

// VARIABLES
extern volatile uint8_t povol_TX;

// FUNCTIONS
void UART_init(void);                       // initialization and initiation communication -- default baud rate is 19600
void ble_send(const char *data);            // send char data
void ble_send_uint16(uint16_t number);      // send uint16_t data

#endif /* UART_H_ */
