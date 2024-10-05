/*
 * LED.c
 *
 *  Created on: 24. 9. 2020
 *      Author: dosedel
 */

#include "include/LED.h"

void LED_init(void)
{
    P1DIR |= 0x0C;  //  P1.2 and P1.3 as output
    P4DIR |= 0x04;  //  P4.2 as output
    P3DIR |= 0x40;  //  P3.6 as output

    //flash the LEDs an the power-up
    led_flash_all(3); // how many times the LEDs blink, 3 = 3 blinks

}

//flash all LEDs n times.
void led_flash_all(uint8_t n) {
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

