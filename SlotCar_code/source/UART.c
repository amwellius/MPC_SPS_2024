/*
 * UART.c
 *
 *  Created on: 28. 9. 2021
 *      Author: 
 */

// INCLUDES
#include "include/UART.h"
#include <stdio.h>  // Include for sprintf

// VARIABLES
volatile uint8_t povol_TX;

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
    int is_negative = 0;

    // Handle negative numbers
    if (number < 0) {
        is_negative = 1;
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
#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
    unsigned char RX_data;


    switch(__even_in_range(UCA1IV,4))
    {
    case 0x00: break;   // Vector 0: No interrupts
    case 0x02:          // Vector 2: Data received; Interrupt Flag: UCRXIFG; Interrupt
         RX_data = (uint8_t)UCA1RXBUF;
         if (UCA1RXBUF == 'z') // 'z' received?
                {
                  // pri prijatem znaku 'z' povol vysilaci komunikaci
                 povol_TX = 1;
                  }
         else if (UCA1RXBUF == 'y')
         {
             povol_TX = 0; //zakaz komunikaci
         }
    break;
    case 0x04: break;  // Vector 4: Transmit buffer empty; Interrupt Flag: UCTXIFG
    default: break;
    }
}

