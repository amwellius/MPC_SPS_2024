/*
 * aux.c
 *
 *  Created on: 22/October/2024
 *      Author: xkosik09
 */

// INCLUDES
#include <msp430.h>
#include "include/timers.h"
#include "include/aux.h"
#include "include/ADC.h"
#include "include/motor.h"
#include "include/LED.h"
#include "include/UART.h"
#include "include/data_temp_storage.h"
#include "include/data_operation.h"

// VARIABLES

// FUNCTIONS

/*
 * Function for very simple controlling of the car speed according to the data from the ADC.
 * The ADC data is processed every ~62.5ms
 */
void car_control_simple(void)
{
    #ifdef axis_enable_x
    uint16_t x_axis = 0;
    #elif defined(axis_enable_y)
    uint16_t y_axis = 0;
    #elif defined(axis_enable_z)
    uint16_t z_axis = 0;
    #endif

    // this happens every ~62.5 ms
    if (flag_62ms)
    {
//        LED_FR_toggle();
        // ADC operation
        // Get the results using the getter function
        #ifdef axis_enable_x
        x_axis = ADC_get_result(2);
        #elif defined(axis_enable_y)
        y_axis = ADC_get_result(3);
        #elif defined(axis_enable_z)
        z_axis = ADC_get_result(4);
        #endif

        #ifdef axis_enable_x
        // up / down
        // doesnt work properly. We dont need this
        switch(x_axis)
        {
        case 0 ... 2300:    // stable
            LED_RL_ON();
            LED_RR_OFF();
            break;
        case 2310 ... 4096: //
            LED_RL_OFF();
            LED_RR_ON();
            break;
        default:
            LED_RL_ON();
            LED_RR_ON();
            break;
        }
        ble_send_uint16(x_axis);
        ble_send("\n");

        // forward / backward
        #elif defined(axis_enable_y)
        switch(y_axis)
        {
        case 0 ... 2125:    // forward
            LED_RL_ON();
            LED_RR_OFF();
            break;
        case 2135 ... 4096: // backwards
            LED_RL_OFF();
            LED_RR_ON();
            break;
        default:
            LED_RL_ON();
            LED_RR_ON();
            break;
        }
        ble_send_uint16(y_axis);
        ble_send("\n");
        #elif defined(axis_enable_z)
        // left / right
        /*
         * looks like middle value it 1964 when the motor is NOT running. 0-1955 for right, 1968-4055 for left.
         * There also might be a bit of offset.
         * PROBLEM: When the motor is running, the ADC data fluctuates - is in noise,
         * so the speed adjusts accordingly. However, this is only noise not a real data reading.
         * NOTE 4 Nov:
         * The motor is running, window filter 1000 samples. The average center value when the motor IS running is ~ 1966
         */
        switch(z_axis)
        {
        case 0 ... 1959:    // momentum vector RIGHT, RIGHT LED ON
            LED_RR_ON();
            LED_RL_OFF();
            motor_pwm(PWM_LEVEL_4);
            break;
        case 1970 ... 4095: // momentum vector LEFT, LEFT LED ON
            LED_RR_OFF();
            LED_RL_ON();
            motor_pwm(PWM_LEVEL_4);
            break;
        default:
            LED_RL_OFF();
            LED_RR_OFF();
            motor_pwm(PWM_LEVEL_5);
            break;
        }
        #endif

        // Send data over UART BLUETOOTH
        ble_send_uint16(z_axis);
        ble_send("\n");

        flag_62ms = 0;
    }
}

/***************************************************************************************************************/
/*
 * A Finite State Machine for controlling the motor PWM speed.
 * In main.c, first initialize the FSM. Then use car_control_FSM in the while look to run indefinitely.
 *
 */

static State current_state = STATE_REF_LAP;

#ifdef FSM_STATE_DBG
    static uint8_t state_counter = 0;
#endif

bool temp_flag = false;

// Initialize the state machine
void state_machine_init(void) {
    current_state = STATE_REF_LAP;
    ble_send("State Machine Initialized: STATE_INIT\n");
}

// Function to run the state machine logic
uint16_t ii = 0;
uint16_t i = 0;
uint16_t z_axis = 0;

void car_control_FSM(void)
{
    switch (current_state) {
        case STATE_REF_LAP:
            #ifdef FSM_STATE_DBG
            if (!temp_flag) {
                ble_send("\n\nFSM START \nIn STATE_REF_LAP\n");
                temp_flag = true;
            }
            #endif

            // set constant speed for reference lap
            motor_pwm(PWM_LEVEL_3);

            // blink rear LEDs while in reference lap
            if (variable_delay_ms(6, 100)) {
                LED_RR_toggle();
                LED_RL_toggle();
            }

            // perform correlation every xx ms
            if (variable_delay_ms(5, 10)) {

                // get ADC results
                z_axis = ADC_get_result(4);

                #ifdef FSM_DBG_SEND_ADC
                // Send data over UART BLUETOOTH
                ble_send_uint16(z_axis);
                ble_send("\n");
                #endif
                #ifdef FSM_DBG
                if (corrDetectNewLapStart(feed_stored_data(stored_track_data_3, STORED_DATA_3_LENGTH))) {
                #endif
                #ifndef FSM_DBG
                if (corrDetectNewLapStart(z_axis)) {
                #endif

                    // BLE DBG
//                    ble_send_int32(850000000);
//                    ble_send("\n");
//                    ble_send("**** Correlation found ****\n");
                    // reset LEDs to OFF state
                    LED_RR_OFF();
                    LED_RL_OFF();
//                    lap_counter();

                    // go to the next state
//                    state_transition(STATE_STOPPED);
                    #ifdef FSM_STATE_DBG
                    temp_flag = false;
                    #endif
                }
            }
            break;

        case STATE_RUNNING:
            #ifdef FSM_STATE_DBG
            if (!temp_flag) {
//                ble_send("In STATE_RUNNING\n");
                temp_flag = true;
            }
            #endif
            if (variable_delay_ms(5, 200)) {
                LED_FR_toggle();
//                ble_send("\nIn STATE_RUNNING\n");
            }

            if (flag_62ms)
            {
                // go thru saved data
                if (i<STORED_DATA_2_LENGTH)
                {
                    // ADC operation
                    // Get the results using the getter function
                    z_axis = stored_track_data_2[i];

                    switch(z_axis)
                    {
                    case 0 ... 1959:    // momentum vector RIGHT, RIGHT LED ON
                        LED_RR_ON();
                        LED_RL_OFF();
                        motor_pwm(PWM_LEVEL_4);
                        break;
                    case 1970 ... 4095: // momentum vector LEFT, LEFT LED ON
                        LED_RR_OFF();
                        LED_RL_ON();
                        motor_pwm(PWM_LEVEL_4);
                        break;
                    default:
                        LED_RL_OFF();
                        LED_RR_OFF();
                        motor_pwm(PWM_LEVEL_5);
                        break;
                    }

                    i++;
                }
                flag_62ms = 0;
                if (i==STORED_DATA_2_LENGTH){
                    state_transition(STATE_STOPPED);
                    temp_flag = false;
                    i = 0;
                    LED_FR_OFF();
                }
            }

//            ble_send("\nToggle FR led.\n");
//            LED_FR_toggle();
//            state_transition(STATE_ERROR);
            break;

        case STATE_STOPPED:
//            ble_send("\nIn STATE_STOPPED\n");

            if (!temp_flag) {
//                ble_send("In STATE_STOPPED\n");
                temp_flag = true;
            }

            static uint8_t temp_counter = 0;
            // blink rear LEDs when in stopped lap
            if (variable_delay_ms(6, 500)) {
                LED_RR_toggle();
                LED_RL_toggle();
//                ble_send("FSM reset in: ");
                ble_send_uint16(20 - temp_counter);
                ble_send("\n");
                temp_counter++;
            }
            // after 30 seconds go back to Ref_lap
            if (temp_counter == 20){
                state_transition(STATE_REF_LAP);
                temp_counter = 0;
                temp_flag = false;
            }


            // Conditions to move to INIT, ERROR, etc.
//            state_transition(10); // unknown state
            break;

        case STATE_ERROR:
            ble_send("\nIn STATE_ERROR\n");
            // Error handling or recovery
            state_transition(STATE_STOPPED);
            break;

        default:
            #ifdef FSM_STATE_DBG
            ble_send("\nUnknown State!\n");
            ble_send("State reset counter: ");
            state_counter++;
            ble_send_uint16(state_counter);
            ble_send(".\n");
            ble_send("*** reset FSM now! *** \n\n");
            state_machine_init();
            #endif
            break;
        }
}

// Function to handle state transitions
void state_transition(State new_state)
{
#ifdef FSM_STATE_DBG
//    ble_send("Transitioning from ");
//    ble_send_uint16(current_state);
//    ble_send(" to ");
//    ble_send_uint16(new_state);
//    ble_send(".\n");
#endif
    current_state = new_state;
}

/*
 * Function to feed stored data into FSM.
 * The first few samples from ADC are dumped. Make sure to check #define DUMP_SAMPLES in data_operations.h
 */
uint16_t feed_stored_data(const uint16_t *storedData, uint16_t dataLength) {
    static uint16_t counter = 0;
    uint16_t feedData = 0;

    // Increment counter if within range
    if (counter <= dataLength) {
        feedData = storedData[counter];
        counter++;
    }
    else { // the data ends here
        counter = 0;
        corrClearBuffers();                     // clear correlation buffers
        ble_send("DGB: Buffers cleared!\n");    // Optional debug output
        feedData = storedData[counter];
        counter++;
    }
    return feedData; // Return the current data value
}

// A function to print laps when called
void lap_counter(void)
{
    static uint8_t lap_counter = 0;
    lap_counter++;
    ble_send("Lap counter: ");
    ble_send_uint16(lap_counter);
    ble_send("\n");
}






// **************************************INTERUPTS************************************** //
