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
//#include "include/data_temp_storage.h"
#include "include/data_operation.h"

// DEFINITIONS
#define MASTER_BLE_DBG      // if defined, LED_FL toggles every 120ms to indicate debug mode

// VARIABLES
bool MASTER_BLE_DBG_flag = false;

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

    // INITS
    LED_init();
    init_timerB0();
    init_timerA1();
    motor_init();
    ADC_init();
    ADC_start();
    UART_init();

    // set 1 to start on start-up
    allow_TX = 0;

    while(1)
    {
        #ifdef MASTER_BLE_DBG
        /* IF IN DEBUG MODE */
            if (!MASTER_BLE_DBG_flag) {
                ble_send("\t **** DEBUG MODE ****\n");
                ble_send("Type 'start' or 'stop' to proceed!\n");
                MASTER_BLE_DBG_flag = true;
            }
            if (variable_delay_ms(0, 120)) {
            LED_FL_toggle(); // Blink LED to indicate software debug mode
            }
        #endif

        if (allow_TX == 1) {
//            LED_FR_ON();
            car_control_FSM();
        }
        else
        {
//            LED_FR_OFF();
            motor_brake();
        }
        }



    // OLD infinite loop
    while (1)
    {
//        car_control_simple();
//        car_control_FSM();

//        if (variable_delay_ms(x, 100)) {
//            // Perform task every xxx ms
//            LED_FL_toggle(); // Example task
//        }
//
//        if (variable_delay_ms(x, 300)) {
//            // Perform another task every xxx ms
//            LED_FR_toggle(); // Example task
//        }



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
            flag_1000ms = 0;
        }





    }
}


//**********************************************************************************************




