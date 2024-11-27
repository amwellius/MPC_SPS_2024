/*
 * UART.c
 *
 *  Created on: 28. 9. 2021
 *      Author: 
 * Modified on: 12/November/2024
 *      Author: xkosik09
 */

// INCLUDES
#include "include/UART.h"
#include <string.h>

// VARIABLES
volatile uint8_t external_control;
char RX_buffer[MAX_RX_BUFFER_SIZE];  // Buffer to store received characters
unsigned int RX_index = 0;        // Current position in the buffer

// FUNCTIONS

/*
 * UART Bluetooth module init function to set it up
 */
void UART_init(void)
{
    UCA1CTL1 = UCSWRST; //UCA1 softwarovy reset -> ON

    P5SEL |= BIT7;
    P5SEL |= BIT6;

    P5OUT &= ~BIT7;
    P5OUT &= ~BIT6;

    UCA1CTL1 |= UCSSEL__SMCLK; //nastaveni zdrojoveho frekvence SMCLK

   // UCA1BR0 = 0x34;           //16MHz / 19200 baud
   //UCA1BR1 = 0x00;
   //UCA1MCTL = 0x11;          //UCOS16 = 1 / UCBRFx = 1 -> dle nastaveni v tabulce datasheetu

   UCA1BR0 = 0x08;           //16MHz / 115200 baud
   UCA1BR1 = 0x00;
   UCA1MCTL = UCBRF_11 + UCOS16; //UCOS16 = 1 / UCBRFx = 11 -> dle nastaveni v tabulce datasheetu
   UCA1CTL1 &= ~UCSWRST;     //UCA1 softwarovy reset -> OFF

   UCA1IE |= UCRXIE;         //Enable USCI_A1 RX interrupt
}
/*
 * Send char data over Bluetooth
 */
void ble_send(const char *data) {
    while (*data) {
        while (!(UCA1IFG & UCTXIFG));  // Wait for the transmit buffer to be ready
        UCA1TXBUF = *data;             // Send the current character
        data++;                        // Move to the next character
    }
}

/*
 * Send uint16_t data over Bluetooth
 */
void ble_send_uint16(uint16_t number) {
    char buffer[6];  // Buffer to hold ASCII string (max 5 digits + null terminator)
    int i = 0;

    // Handle 0 case explicitly
    if (number == 0) {
        ble_send("0");
        return;
    }

    // Extract digits from the number (from least significant to most significant)
    while (number > 0) {
        buffer[i++] = (number % 10) + '0';  // Convert digit to ASCII
        number /= 10;
    }

    // Reverse and send the digits since they were stored in reverse
    while (i > 0) {
        ble_send((char[]){buffer[--i], '\0'});  // Send one digit at a time
    }
}

/*
 * Send int16_t data over Bluetooth
 */
void ble_send_int16(int16_t number) {
    char buffer[7];  // Buffer to hold ASCII string (max 6 digits + sign + null terminator)
    int i = 0;

    // Handle negative numbers
    if (number < 0) {
        number = -number;  // Take the absolute value
        ble_send("-");     // Send the negative sign
    }

    // Handle 0 case explicitly
    if (number == 0) {
        ble_send("0");
        return;
    }

    // Extract digits from the number (from least significant to most significant)
    while (number > 0) {
        buffer[i++] = (number % 10) + '0';  // Convert digit to ASCII
        number /= 10;
    }

    // Reverse and send the digits since they were stored in reverse
    while (i > 0) {
        ble_send((char[]){buffer[--i], '\0'});  // Send one digit at a time
    }
}

/*
 * Send int32_t data over Bluetooth
 */
void ble_send_int32(int32_t number) {
    char buffer[12];  // Buffer to hold ASCII string (max 10 digits + sign + null terminator)
    int i = 0;

    // Handle negative numbers
    if (number < 0) {
        number = -number;  // Take the absolute value
        ble_send("-");     // Send the negative sign
    }

    // Handle 0 case explicitly
    if (number == 0) {
        ble_send("0");
        return;
    }

    // Extract digits from the number (from least significant to most significant)
    while (number > 0) {
        buffer[i++] = (number % 10) + '0';  // Convert digit to ASCII
        number /= 10;
    }

    // Reverse and send the digits since they were stored in reverse
    while (i > 0) {
        ble_send((char[]){buffer[--i], '\0'});  // Send one digit at a time
    }
}

// **************************************INTERUPTS************************************** //
// Command line session handlings
#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
    char RX_data;

    switch(__even_in_range(UCA1IV, 4)) {
        case 0x00: break;   // No interrupt

        case 0x02:          // Data received
            RX_data = UCA1RXBUF;  // Read received data

            if (RX_data == '\n') {  // Check for end of command
                RX_buffer[RX_index] = '\0';  // Null-terminate the string

                // Check for commands
                if (strcmp(RX_buffer, "help") == 0) { // include all external BLE commands
                    ble_send("Commands:\n");
                    ble_send("'start' \t 'stop' \t 'reset' \t 'map' \t 'help' \t 'about'\n");
                }
                else if (strcmp(RX_buffer, "start") == 0) {
                    ble_send("...running...\n");
                    external_control = 1;
                } else if (strcmp(RX_buffer, "stop") == 0) {
                    ble_send("...paused...\n");
                    external_control = 0;
                } else if (strcmp(RX_buffer, "reset") == 0) {
                    ble_send("...resetting...\n");
                    external_control = 2;
                } else if (strcmp(RX_buffer, "map") == 0) {
                    external_control = 3;
                } else if (strcmp(RX_buffer, "about") == 0) {
                    ble_send("----------------------------------------------------------------------\n");
                    ble_send("MPC_SPS_2024 course CarControlProject\n");
                    ble_send("Copyrights: Alzbeta Kostelanska, Samuel Kosik\n");
                    ble_send("BUT, BRNO, Czechia, 2025, all rights reserved.\n");
                    ble_send("Copyright (c) 2024, MIT License\n");
                    ble_send("-----------------------------------------------------------------------\n");
                }
                /* Add more commands here */

                // Reset buffer for the next command
                RX_index = 0;
            } else {
                // Store character in buffer if there is space
                if (RX_index < MAX_RX_BUFFER_SIZE - 1) {
                    RX_buffer[RX_index++] = RX_data;
                } else {
                    RX_index = 0;  // Reset if buffer overflows
                }
            }
            break;

        case 0x04: break;  // Transmit buffer empty
        default: break;
    }
}

