/*
 * UART.c
 *
 *  Created on: 28. 9. 2021
 *      Author: 
 */


#include "include/UART.h"

volatile uint8_t povol_TX;

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


// preruseni pro prijem dat
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

