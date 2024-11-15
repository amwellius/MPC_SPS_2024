/*
 * timers.c
 *
 *  Created on: 05/October/2024
 *      Author: xkosik09
 */

// INCLUDES
#include "include/timers.h"
#include <msp430.h>
#include "include/LED.h"

// VARIABLES
volatile unsigned char flag_1ms = 0;            // Define flag_1ms
volatile unsigned char flag_31ms = 0;           // Define flag_31.75ms
volatile unsigned char flag_62ms = 0;           // Define flag_62.5ms
volatile unsigned char flag_500ms = 0;          // Define flag_500ms
volatile unsigned char flag_1000ms = 0;         // Define flag_1000ms
static uint16_t overflow_count5 = 0;            // Incremented every 1 ms

/* main clock 16 MHz
 * This function doesn’t set up the timer directly but ensures your system clock is running at 16 MHz.
 */
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

/* Timer A, instance 0. This function generates 20KHz PWM signal for the motor.
 It only driver the PHASE pin, P8.2 - DIRECTION.
 50%  = off
 0%   = forward full speed
 100% = reverse full speed
*/
void init_timerA0(uint16_t duty_cycle)
{
    P8DIR |= BIT2;                     // Set P8.2 as output for PWM signal (TA0.2)
    P8SEL |= BIT2;                     // Select Timer_A function for P8.2

    TA0CCR0 = 800;                     // Set the period for 20 kHz PWM
    TA0CCTL2 = OUTMOD_7;               // Set/reset mode for TA0.2

    // Set duty cycle (400 = 50%; 600 = 75%; 200 = 25%)
    if (duty_cycle <= TA0CCR0)
    {                                   // Ensure duty cycle doesn't exceed period
        TA0CCR2 = duty_cycle;           // Set new duty cycle
    }

    TA0CTL = TASSEL_2 + MC_1;          // SMCLK, Up mode
}

// Timer B, instance 0. This function sets up the timer to count up to CCR0 level.
void init_timerB0(void)
{
    TB0CCR0 = 65535;                    // Set the maximum count (65535)
    TB0CCTL0 |= CCIE;                   // Enable interrupt for CCR0
    TB0CTL = TASSEL_2 + MC_1 + ID_3;    // SMCLK, Up mode, Prescaler = 8 (ID_3)
}

// Timer A1 Initialization for millisecond delay
void init_timerA1(void)
{
    TA1CCR0 = 16000 - 1;             // Set Timer A1 period for 1 ms (assuming 16 MHz clock)
    TA1CTL = TASSEL_2 + MC_1;        // SMCLK, Up mode
    TA1CCTL0 |= CCIE;                // Enable Timer A1 interrupt
}

/*
 * Function to set variable delays
 * Usage of this variable delay function:
 *
 *  if (variable_delay_ms(1, 100)) {
 *      // Perform another task every 100 ms
 *      LED_FR_toggle(); // Example task
 *  }
 *
 *  if (variable_delay_ms(1, 300)) {
 *      // Perform another task every 300 ms
 *      LED_FR_toggle(); // Example task
 *  }
 *
 *  *** DO NOT USE FOLLOWING reserved INDEXES:
 *      0, 1, 2, 3, 4
 *      Use only indexes => 5 for test
 *
 *
 */
bool variable_delay_ms(uint8_t index, uint16_t delay_ms)
{
    static uint16_t variable_ms[max_variable_delays] = {0};          // Array to store delay intervals
    static uint16_t reference_count[max_variable_delays] = {0};      // Array to store last reference counts

    if (index >= max_variable_delays) return false; // Ensure index is valid

    // Update the delay if it's different from the stored value
    if (variable_ms[index] != delay_ms) {
        variable_ms[index] = delay_ms; // Update the delay
        reference_count[index] = overflow_count5; // Set reference count to current
        return false; // Return false if a new delay is set
    }

    // Check if the specified delay has elapsed
    if ((overflow_count5 - reference_count[index]) >= delay_ms) {
        reference_count[index] = overflow_count5; // Update reference count to current
        return true; // Delay elapsed
    }

    return false; // Delay not elapsed
}



// **************************************INTERUPTS************************************** //


// Interrupt of Timer B0
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer_B0(void)
{
    static uint8_t overflow_count1 = 0;        // Define overflow_count1
    static uint8_t overflow_count2 = 0;        // Define overflow_count2
    static uint8_t overflow_count3 = 0;        // Define overflow_count3
    static uint8_t overflow_count4 = 0;        // Define overflow_count4

    overflow_count1++;                      // Increment overflow counter 1
    overflow_count2++;                      // Increment overflow counter 2
    overflow_count3++;                      // Increment overflow counter 3
    overflow_count4++;                      // Increment overflow counter 4

    if (overflow_count1 >= half_sec) {
        flag_500ms = 1;                     // Set flag for 0.5 sec
        overflow_count1 = 0;                // Reset counter
    }
    if (overflow_count2 >= one_sec) {
        flag_1000ms = 1;                    // Set flag for 1 sec
        overflow_count2 = 0;                // Reset counter
    }
    if (overflow_count3 >= sixtytwo_msec ) {
        flag_62ms = 1;                      // Set flag for ~62.5 ms
        overflow_count3 = 0;                // Reset counter
    }
    if (overflow_count4 >= thirdy_one_msec ) {
        flag_31ms = 1;                      // Set flag for ~31.75 ms
        overflow_count4 = 0;                // Reset counter
    }

}

// Interrupt of Timer A1
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer_A1(void)
{
    static uint8_t  overflow_count6 = 0; // Define overflow_count4
    static uint16_t  temp_counter = 0;

    overflow_count5++;                   // Increment overflow counter 5 every 1ms for the various delay function
    overflow_count6++;                   // Increment overflow counter 6

    if (overflow_count6 >= one_ms) {
        flag_1ms = 1;                    // Set flag for 1 ms
        overflow_count6 = 0;             // Reset counter
    }

    // temp counter for brakes
    bool static start_counting = false;

    if (restart_counter) {
        temp_counter = 0;
        restart_counter = 0;
        start_counting = true;

    }

    if (start_counting) {
        temp_counter++;
        if (temp_counter >= 400) {
            start_counting = false;
            LED_RR_OFF();
            LED_RL_OFF();
        }
    }


}





