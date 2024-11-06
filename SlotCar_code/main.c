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

// VARIABLES


//// tests //////
#define KERNEL_SIZE 8  // Define based on expected pattern size
#define THRESHOLD -2000 // Example threshold; adjust based on testing
#define DEBOUNCE_COUNT 3  // Number of consistent readings before state change

// Define your convolution kernel
const int16_t kernel[KERNEL_SIZE] = {0, 0, 0, -1, -1, 0, 0, 0}; // Example kernel for bends



// Function to perform convolution on the ADC data
int16_t perform_convolution(uint16_t* data, uint32_t length, uint32_t index) {
    int16_t conv_result = 0;
    uint8_t i = 0;

    // Apply kernel only if there's enough data to compute it
    if (index >= KERNEL_SIZE / 2 && index < length - KERNEL_SIZE / 2) {
        for (i = 0; i < KERNEL_SIZE; i++) {
            conv_result += data[index - KERNEL_SIZE / 2 + i] * kernel[i];
        }
    }
    return conv_result;
}



// Function to determine if the car is in a bend or straight section
void analyze_track_section(uint16_t* data, uint32_t length) {
    uint32_t i = 0;
    uint8_t bend_count = 0;
    int16_t conv_value;
    uint8_t length_temp = 20;

    for (i = 0; i < length; i++) {
        conv_value = perform_convolution(data, length, i);
//        ble_send("FINAL conv_value: ");
        ble_send_int16(conv_value);
        ble_send("\n");
//        ble_send("\n");

        // Check if conv_value exceeds threshold consistently
        if (conv_value > THRESHOLD) {
            bend_count++;
            ble_send("\nfind a HIGHT value.\n ");
            if (bend_count >= DEBOUNCE_COUNT) {
//                state_transition(STATE_SLOW_DOWN);  // Example state transition for bends
                bend_count = 0; // Reset after transition
            }
        } else {
            bend_count = 0;

//            state_transition(STATE_NORMAL_SPEED);  // Straight path state transition
        }

        if (conv_value < THRESHOLD) {
                    ble_send("\nfind a low value - two low values.\n ");
                }
    }

}
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
//    ble_send("\nKERNEL: 1 0 -1 0 0 1\n");
//    analyze_track_section(adc_data,SAMPLE_COUNT);
    while(1){

        if (variable_delay_ms(1, 400)) {
            // Perform another task every xxx ms
            analyze_track_section(adc_data,SAMPLE_COUNT);
            LED_FR_toggle(); // Example task
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




