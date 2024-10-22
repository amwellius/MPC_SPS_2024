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
//uint16_t i, index;



// MAIN LOOP
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    initClockTo16MHz();         // initialize clock timer 16 MHz
    __bis_SR_register(GIE);     // Enable global interrupts
    _BIS_SR(GIE);        // some interrupts as well


    LED_init();
    init_timerB0();
    motor_init();
    ADC_init();
    ADC_start();
    UART_init();



    // infinite loop
    while (1) {
        car_control_simple();


        if (flag_500ms) {               // Check if flag is set
//            LED_FR_toggle();
            flag_500ms = 0;             // Clear the flag
        }

        if (flag_1000ms) {
            LED_FL_toggle();

            // Sending data BLUETOOTH
//            if (povol_TX == 1)
//            {
//                UCA1TXBUF = z_axis;   // data jsou ihned po vlozeni do UCAxTXBUF odeslana na seriovou branu
////                UCA1TXBUF = 77; // test letter M ASCII
//
//                /*
//                 * clean up the code - create aux.c file?
//                 * ADC better range
//                 * look into command line
//                 *
//                 */
//            }

            flag_1000ms = 0;
        }





    }
}


//**********************************************************************************************




