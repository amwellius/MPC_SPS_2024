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
#include "include/flash_memory.h"
#include "include/data_temp_storage.h"
#include "include/data_operation.h"

// VARIABLES


//// tests //////

//// tests //////

// MAIN LOOP
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    initClockTo16MHz();         // initialize clock timer 16 MHz
    __bis_SR_register(GIE);     // Enable global interrupts
    _BIS_SR(GIE);               // some interrupts as well
    __enable_interrupt();


    LED_init();
    init_timerB0();
    init_timerA1();
//    motor_init();
//    ADC_init();
//    ADC_start();
    UART_init();


    //// tests //////
//    uint16_t i = 0;
//    ble_send("\n \t *** STARTING STATE MACHINE ***\n\n");
//    state_machine_init();  // Initialize the state machine
    //// tests //////

//    while (1)
//    {
        //// tests //////
//        car_control_FSM();
        //// tests //////

//        if (variable_delay_ms(1, 1000)) {
////            LED_FR_toggle(); // Example task
//        }
//
//
//        if (variable_delay_ms(2, 62)) {
//
//            if (i<SAMPLE_COUNT)
//            {
////                ble_send_uint16(adc_data[i]);
////                ble_send("\n");
//                i++;
//            }
//
//            switch(adc_data[i])
//            {
//            case 0 ... 1959:    // momentum vector RIGHT, RIGHT LED ON
//                LED_RR_ON();
//                LED_RL_OFF();
//                motor_pwm(PWM_LEVEL_4);
//                break;
//            case 1970 ... 4095: // momentum vector LEFT, LEFT LED ON
//                LED_RR_OFF();
//                LED_RL_ON();
//                motor_pwm(PWM_LEVEL_4);
//                break;
//            default:
//                LED_RL_OFF();
//                LED_RR_OFF();
//                motor_pwm(PWM_LEVEL_5);
//                break;
//            }
//
//            if (i==SAMPLE_COUNT)
//            {
//                i = 0;
//            }
//            LED_FL_toggle();
//
//        }

//    }

//    car_control_FSM();


    //// tests //////

    uint16_t ii = 0;
    while(1){

        if (variable_delay_ms(5, 10)) {
        // Perform task every xxx ms
            LED_FR_toggle(); // Example task
            if (ii<STORED_DATA_1_LENGTH) {
                corrCollectADCData(stored_track_data_1[ii]);
//                ble_send("loops: ");
//                ble_send_uint16(ii);
//                ble_send("\n");
                ii++;
            }
            if (ii == STORED_DATA_1_LENGTH)
            {
                ble_send_int32(79000000);
                ble_send("\n");
                ii = 0;
                corrClearBuffers();
//                ble_send("\na loop thru the data samples!\n");
            }
        }

        if (variable_delay_ms(6, 500)) {
        LED_FL_toggle(); // Example task
        }
    }

    //// tests //////









    // infinite loop
    while (1)
    {
        car_control_simple();
//        car_control_FSM();

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




