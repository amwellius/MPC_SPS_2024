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

// VARIABLES

// FUNCTIONS

/*
 * Function for very simple controlling of the car speed according to the data from the ADC.
 * The ADC data is processed every ~62.5ms
 */
void car_control_simple(void)
{
    uint16_t x_axis = 0;
    uint16_t y_axis = 0;
    uint16_t z_axis = 0;

    // this happens every ~62.5 ms
    if (flag_62ms)
    {
//        LED_FR_toggle();
        // ADC operation
        // Get the results using the getter function
        x_axis = ADC_get_result(2);
        y_axis = ADC_get_result(3);
        z_axis = ADC_get_result(4);

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

/*
 * A Finite State Machine for controlling the motor PWM speed.
 * In main.c, first initialize the FSM. Then use car_control_FSM in the while look to run indefinitely.
 *
 */

static State current_state = STATE_INIT;
static uint8_t state_counter = 0;

// Initialize the state machine
void state_machine_init(void) {
    current_state = STATE_INIT;
    ble_send("State Machine Initialized: STATE_INIT\n");
}

// Function to run the state machine logic
void car_control_FSM(void)
{
    if (variable_delay_ms(3, 1000))
    {
        switch (current_state) {
            case STATE_INIT:
                ble_send("\nIn STATE_INIT\n");
                // Example: Transition to RUNNING
                state_transition(STATE_RUNNING);
                break;

            case STATE_RUNNING:
                ble_send("\nIn STATE_RUNNING\n");
                // Add conditions to transition to STOPPED or other states
                ble_send("\nToggle FR led.\n");
                LED_FR_toggle();
                state_transition(STATE_ERROR);
                break;

            case STATE_STOPPED:
                ble_send("\nIn STATE_STOPPED\n");
                // Conditions to move to INIT, ERROR, etc.
                state_transition(10); // unknown state
                break;

            case STATE_ERROR:
                ble_send("\nIn STATE_ERROR\n");
                // Error handling or recovery
                state_transition(STATE_STOPPED);
                break;

            default:
                ble_send("\nUnknown State!\n");
                ble_send("State reset counter: ");
                state_counter++;
                ble_send_uint16(state_counter);
                ble_send(".\n");
                ble_send("*** reset FSM now! *** \n\n");
                state_machine_init();
                break;
            }
    }
}

// Function to handle state transitions
void state_transition(State new_state) {
    ble_send("Transitioning from ");
    ble_send_uint16(current_state);
    ble_send(" to ");
    ble_send_uint16(new_state);
    ble_send(".\n");
    current_state = new_state;
}







// **************************************INTERUPTS************************************** //
