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

// VARIABLES
uint16_t i, index;



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

    uint16_t x_axis = 0;
    uint16_t y_axis = 0;
    uint16_t z_axis = 0;

    // infinite loop
    while (1) {
        if (flag_62ms) {
            LED_FR_toggle();
            // ADC operation
            // Get the results using the getter function
            x_axis = ADC_get_result(2);
            y_axis = ADC_get_result(3);
            z_axis = ADC_get_result(4);

            // left / right
            switch(z_axis)
            {
            case 0 ... 1952:    // left
                LED_RL_ON();
                LED_RR_OFF();
                motor_pwm(PWM_LEVEL_4);
                break;
            case 1962 ... 4096: // right
                LED_RL_OFF();
                LED_RR_ON();
                motor_pwm(PWM_LEVEL_4);
                break;
            default:
                LED_RL_OFF();
                LED_RR_OFF();
                motor_pwm(PWM_LEVEL_6);
                break;
            }
            flag_62ms = 0;
        }

        if (flag_500ms) {               // Check if flag is set
//            LED_FR_toggle();
//            motor_pwm(PWM_LEVEL_1);
            flag_500ms = 0;             // Clear the flag
        }

        if (flag_1000ms) {
            LED_FL_toggle();
//            motor_pwm(PWM_LEVEL_2);

            // Sending data BLUETOOTH
            if (povol_TX == 1)
            {
                UCA1TXBUF = z_axis;   // data jsou ihned po vlozeni do UCAxTXBUF odeslana na seriovou branu
//                UCA1TXBUF = 77; // test letter M ASCII

                /*
                 * clean up the code - create aux.c file?
                 * ADC better range
                 * look into command line
                 *
                 */
            }

            flag_1000ms = 0;
        }




        // forward / backward
//        switch(y_axis)
//        {
//        case 0 ... 2125:    // forward
//            LED_RL_ON();
//            LED_RR_OFF();
//            break;
//        case 2135 ... 4096: // backwards
//            LED_RL_OFF();
//            LED_RR_ON();
//            break;
//        default:
//            LED_RL_ON();
//            LED_RR_ON();
//            break;
//        }

        // up / down
        // doenst work properly. We dont need this
//        switch(x_axis)
//        {
//        case 0 ... 2300:    // stable
//            LED_RL_ON();
//            LED_RR_OFF();
//            break;
//        case 2310 ... 4096: //
//            LED_RL_OFF();
//            LED_RR_ON();
//            break;
//        default:
//            LED_RL_ON();
//            LED_RR_ON();
//            break;
//        }
    }
}


//**********************************************************************************************




