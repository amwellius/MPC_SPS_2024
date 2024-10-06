/*
 * timers.c
 *
 *  Created on: 05/October/2024
 *      Author: xkosik09
 */

#include "include/timers.h"
#include <msp430.h>

// Variables
volatile unsigned int overflow_count1 = 0;      // Define overflow_count1
volatile unsigned int overflow_count2 = 0;      // Define overflow_count2
volatile unsigned char flag_500ms = 0;          // Define flag_500ms
volatile unsigned char flag_1000ms = 0;          // Define flag_1000ms

// Definitions
#define half_sec    16  // 16 = Approximately 0.5 seconds
#define one_sec     32  // 32 = Approximately 1 seconds


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
//void init_timerA0(void)
//{
//    TA0CCR0 = 65535;                    // Set the maximum count (65535)
//    TA0CCTL0 |= CCIE;                   // Enable interrupt for CCR0
//    TA0CTL = TASSEL_2 + MC_1 + ID_3;    // SMCLK, Up mode, Prescaler = 8 (ID_3)
//}

void init_timerA0(void)
{
    P8DIR |= BIT2;                     // Set P8.2 as output for PWM signal (TA0.2)
    P8SEL |= BIT2;                     // Select Timer_A function for P8.2

    TA0CCR0 = 800;                     // Set the period for 20 kHz PWM
    TA0CCTL2 = OUTMOD_7;               // Set/reset mode for TA0.2
    TA0CCR2 = 00;                     // Set duty cycle (50%)

    TA0CTL = TASSEL_2 + MC_1;          // SMCLK, Up mode
}

void init_timerA1(void)
{
    P1DIR |= BIT2;                     // Set P1.2 as output for PWM signal (TA1.1)
    P1SEL |= BIT2;                     // Select Timer_A function for P1.2

    TA1CCR0 = 800;                     // Set the period for 20 kHz PWM
    TA1CCTL1 = OUTMOD_7;               // Set/reset mode for TA1.1
    TA1CCR1 = 600;                     // Set duty cycle (400 = 50%; 600 = 75%; 200 = 25%)

    TA1CTL = TASSEL_2 + MC_1;          // SMCLK, Up mode
}

void init_timerB0(void)
{
    P4DIR |= BIT2;                     // Set P4.2 as output for PWM signal (TB0.2)
    P4SEL |= BIT2;                     // Select Timer_B function for P4.2

    TB0CCR0 = 800;                     // Set the period for 20 kHz PWM
    TB0CCTL2 = OUTMOD_7;               // Set/reset mode for TB0.2
    TB0CCR2 = 100;                     // Set duty cycle (400 = 50%; 600 = 75%; 200 = 25%)

    TB0CTL = TASSEL_2 + MC_1;          // SMCLK, Up mode
}



// ***************************************************************************** //
// INTERUPTS
//#pragma vector = TIMER0_A0_VECTOR
//__interrupt void Timer_A(void)
//{
//    overflow_count1++;                   // Increment overflow counter 1
//    overflow_count2++;                   // Increment overflow counter 2
//    if (overflow_count1 >= half_sec) {
//        flag_500ms = 1;                 // Set flag for 0.5 sec
//        overflow_count1 = 0;             // Reset counter
//    }
//    if (overflow_count2 >= one_sec) {
//        flag_1000ms = 1;                 // Set flag for 1 sec
//        overflow_count2 = 0;             // Reset counter
//    }
//
//}





