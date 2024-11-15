/*
 * LED.c
 *
 *  Created on: 23/September/2024
 *      Author: xkosik09
 */

// INCLUDES
#include "include/LED.h"
#include "include/timers.h"

volatile unsigned char restart_counter = 0;

// FUNCTIONS
void LED_init(void)
{
    P1DIR |= 0x0C;  //  P1.2 and P1.3 as output
    P4DIR |= 0x04;  //  P4.2 as output
    P3DIR |= 0x40;  //  P3.6 as output

    //flash the LEDs an the power-up
    led_flash_all(3); // how many times the LEDs blink, 3 = 3 blinks

}

//flash all LEDs n times.
void led_flash_all(uint8_t n)
{
    // set all LEDs off
    LED_FR_OFF();
    LED_FL_OFF();
    LED_RR_OFF();
    LED_RL_OFF();

    // loop for toggling the LEDs
    uint8_t i;
    for (i=0; i<n * 2; i++) {
        LED_FR_toggle();
        LED_FL_toggle();
        LED_RR_toggle();
        LED_RL_toggle();
        __delay_cycles(2000000); // blocking delay
    }
}

// turn rear LEDs on to indicate braking. Turn the LEDs off after xx ms to indicate releasing brakes
void led_brake(void)
{
    // turn LEDs on on each call
    LED_RR_ON();
    LED_RL_ON();
    restart_counter = 1;

    // this does not work properly hence commented out
    // turn LEDs off after xx ms to indicate brakes released
//    if (variable_delay_ms(9, 300)) {
//        LED_RR_OFF();
//        LED_RL_OFF();
//    }
}

