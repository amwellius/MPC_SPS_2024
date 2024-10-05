/*
 * timers.c
 *
 *  Created on: 05/October/2024
 *      Author: xkosik09
 */

#include "include/timers.h"
#include <msp430.h>
#include "include/LED.h"

// Variables
volatile unsigned int overflow_count = 0;      // Define overflow_count
volatile unsigned char motor_flag = 0;          // Define motor_flag

// main clock 16 MHz
void initClockTo16MHz()
{
    UCSCTL3 |= SELREF_2;                      // Set DCO FLL reference = REFO
    UCSCTL4 |= SELA_2;                        // Set ACLK = REFO
    __bis_SR_register(SCG0);                  // Disable the FLL control loop
    UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_5;                      // Select DCO range 16MHz operation
    UCSCTL2 = FLLD_0 + 487;                   // Set DCO Multiplier for 16MHz
                                              // (N + 1) * FLLRef = Fdco
                                              // (487 + 1) * 32768 = 16MHz
                                              // Set FLL Div = fDCOCLK
    __bic_SR_register(SCG0);                  // Enable the FLL control loop

    // Worst-case settling time for the DCO when the DCO range bits have been
    // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
    // UG for optimization.
    // 32 x 32 x 16 MHz / 32,768 Hz = 500000 = MCLK cycles for DCO to settle
    __delay_cycles(500000);//
    // Loop until XT1,XT2 & DCO fault flag is cleared
    do
    {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG); // Clear XT2,XT1,DCO fault flags
        SFRIFG1 &= ~OFIFG;                          // Clear fault flags
    }while (SFRIFG1&OFIFG);                         // Test oscillator fault flag
}


// test function
void test_timer(void) {
    TA0CCR0 = 65535;                    // Set the maximum count (65535)
    TA0CCTL0 |= CCIE;                   // Enable interrupt for CCR0
    TA0CTL = TASSEL_2 + MC_1 + ID_3;    // SMCLK, Up mode, Prescaler = 8 (ID_3)
}


// INTERUPTS
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    overflow_count++;                   // Increment overflow counter
    if (overflow_count >= 16)           // Approximately 0.5 seconds
    {
        motor_flag = 1;                 // Set flag to run the motor function
        LED_FR_toggle();
        overflow_count = 0;             // Reset counter
    }
}





