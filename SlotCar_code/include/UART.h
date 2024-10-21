/*
 * UART.h
 *
 *
 *  Created on: 28. 9. 2021
 *      Author: 
 */
#ifndef UART_H_
#define UART_H_

#include "stdint.h"
#include <msp430.h>

void UART_init(void);     //initialization and initiation communication -- default baud rate is 19600

extern volatile uint8_t povol_TX;


#endif /* UART_H_ */
