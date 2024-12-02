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

    // set 1 to start on start-up, 0 to wait for external command-line input
    external_control = 0;

    // Command-line controlled main infinite while loop
    while(1)
    {
        #ifdef MASTER_BLE_DBG
        /* IF IN DEBUG MODE */
            if (!MASTER_BLE_DBG_flag) {
                ble_send("\t **** DEBUG MODE ****\n");
                ble_send("Type 'help' to proceed!\n");
                MASTER_BLE_DBG_flag = true;
            }
            if (variable_delay_ms(0, 120)) {
//            LED_FL_toggle(); // Blink LED to indicate software debug mode
            }
        #endif

        // Command line external control
        switch (external_control) {
            case 0: // stop state
            {
                motor_brake();
                break;
            }
            case 1: // running state
            {
                car_control_FSM();      // main function program
                break;
            }
            case 2: // reset state
            {
                external_control = 1; // set state to run again after performing reset
                ADC_stop();             // stop ADC
                ADC_init();             // init ADC
                ADC_start();            // start ADC
                dump_map();             // clear map
                state_machine_reset();  // set car FSM to restart mode
                reset_flag = true;      // set reset_flag to TRUE
                segmentsCount = 0;      // reset counter for map segments
                break;
            }
            case 3: // show map state
            {
               show_map();
               external_control = 0; // go to stop state
               break;
            }
            case 4: // show map state
            {
               show_map_segments();
               external_control = 0; // go to stop state
               break;
            }
            default:
            {
                ble_send("ERROR: Unknown state of external BLE control!\n");
                break;
            }
        }
    }
}


//**********************************************************************************************




