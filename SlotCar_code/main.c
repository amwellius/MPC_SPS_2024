/**
 * main.c
 */

// INCLUDES
#include <msp430.h>
#include "include/timers.h"
#include "include/SPI.h"
#include "include/I2C.h"
#include "include/LED.h"
#include "include/ADC.h"
#include "include/L3GD20H.h"
#include "include/ADXL343.h"
#include "include/motor.h"
#include "include/UART.h"
#include "include/aux.h"

// VARIABLES

// MAIN LOOP
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    initClockTo16MHz();         // initialize clock timer 16 MHz
    __bis_SR_register(GIE);     // Enable global interrupts
    _BIS_SR(GIE);        // some interrupts as well
    __enable_interrupt();


    LED_init();
    init_timerB0();
    init_timerA1();
    motor_init();
    ADC_init();
    ADC_start();
    UART_init();

    volatile uint32_t temp_data = 0;

    // infinite loop
    while (1) {

//        delay_ms(200);
//        LED_FL_toggle();

        car_control_simple();

        if (variable_delay_ms(0, 100)) {
            // Perform task every xxx ms
            LED_FL_toggle(); // Example task
        }

        if (variable_delay_ms(1, 300)) {
            // Perform another task every xxx ms
            LED_FR_toggle(); // Example task
        }



        // 1ms interrupt
        if (flag_1ms) {               // Check if flag is set
//            temp_data = moving_average();
            flag_1ms = 0;             // Clear the flag
        }

        // 31.75ms interrupt
        if (flag_31ms) {               // Check if flag is set
//            LED_FL_toggle();
            flag_31ms = 0;             // Clear the flag
        }

        // 500ms interrupt
        if (flag_500ms) {               // Check if flag is set
//            ble_send("Every 500ms\n");
//            ble_send("\n");
            flag_500ms = 0;             // Clear the flag
        }

        // 1000ms interrupt
        if (flag_1000ms) {
            if (povol_TX == 1)
            {
            }

            flag_1000ms = 0;
        }





    }
}


//**********************************************************************************************




