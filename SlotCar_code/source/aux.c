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

/***************************************************************************************************************/
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
 * In main.c, first initialize the FSM. Then use car_control_FSM in the while loop to run indefinitely.
 *
 */

static State current_state = STATE_REF_LAP;
static bool init_FSM_flag = true;
#ifdef FSM_STATE_DBG
static bool state_FSM_flag1= true;
static bool state_FSM_flag2= true;
static bool state_FSM_flag3= true;
static bool state_FSM_flag4= true;
static bool state_FSM_flag5= true;
#endif

static uint16_t mapIndex = 0;       // set init map index to 0

// Define map length
MapPoint trackMap[MAP_SAMPLES_LENGTH]; // Example for 1000 samples max

// Initialize the state machine
void state_machine_init(void) {
    // set flags to true
    init_FSM_flag   = true;
    #ifdef FSM_STATE_DBG
    state_FSM_flag1 = true;
    state_FSM_flag2 = true;
    state_FSM_flag3 = true;
    state_FSM_flag4 = true;
    state_FSM_flag5 = true;
    #endif

    current_state = STATE_REF_LAP;

//    current_state = STATE_DEBUG;    // debugging in process

    ble_send("\nState Machine Initialized: STATE_INIT\n");
}

// Function to run the state machine logic
void car_control_FSM(void)
{
    // init FSM only the first call
    if (init_FSM_flag) {
        state_machine_init();
        init_FSM_flag = false;
    }

    uint16_t z_axis = 0;
    switch (current_state) {
        case STATE_REF_LAP:
        {
            #ifdef FSM_STATE_DBG
            if (state_FSM_flag1) {
                ble_send("In STATE_REF_LAP\n");
                state_FSM_flag1 = false;
            }
            #endif

            // set constant speed for reference lap
            motor_pwm(PWM_LEVEL_5); // means ~1 m/s

            // blink rear LEDs while in reference lap
            if (variable_delay_ms(6, 300)) {
                LED_RR_toggle();
                LED_RL_toggle();
            }

            // perform correlation every xx ms
            // use 10ms for stored data
            // test usage of 62ms for ADC to match the speed control
                // if CORRELATION_WINDOW=100, then this takes samples of 5 seconds of the track
            if (variable_delay_ms(5, 62)) {

                // get ADC results
                z_axis = ADC_get_result(4);

                /*
                 * Create map
                 */

                save_to_map(z_axis);      // save to map




                #ifdef FSM_DBG_SEND_ADC
                // Send data over UART BLUETOOTH
                ble_send_uint16(z_axis);
                ble_send("\n");
                #endif
                #ifdef FSM_DBG
                if (corrDetectNewLapStart(feed_stored_data(stored_track_data_1, STORED_DATA_1_LENGTH))) {
                #endif
                #ifndef FSM_DBG
                if (corrDetectNewLapStart(z_axis)) {
                #endif
                    #ifdef FSM_STATE_DBG
                    ble_send("**** Correlation found ****\n");
                    #endif

                    //reset LEDs to OFF state
                    LED_RR_OFF();
                    LED_RL_OFF();
                    lap_counter();  // count laps function

                    // go to the next state
                    state_transition(STATE_STOPPED);
//                    state_transition(STATE_RUNNING);
                }
            }
            break;
        }
        case STATE_RUNNING:
        {
            #ifdef FSM_STATE_DBG
            if (state_FSM_flag2) {
                ble_send("In STATE_RUNNING\n");
                state_FSM_flag2 = false;
            }
            #endif

            // set flags for turns
            static bool right_turn_flag = true;
            static bool left_turn_flag = true;

            // adjust the time hence samples taken every second with variable delays
            if (flag_62ms) {
                #ifdef FSM_DBG
                z_axis = feed_stored_data(stored_track_data_4, STORED_DATA_4_LENGTH);
                #endif
                #ifndef FSM_DBG
                z_axis = ADC_get_result(4);
                #endif

                switch(z_axis)
                {
                case 0 ... 1959:    // momentum vector RIGHT, RIGHT LED ON
                    LED_FR_ON();
                    LED_FL_OFF();

                    // Call led_brake() to indicate slowing down
                    ble_send("right turn\n");
                    if (right_turn_flag) {
                        led_brake();
                        left_turn_flag = true;
                        ble_send("**right turn first time\n");
                        right_turn_flag = false;
                    }

                    motor_pwm(PWM_LEVEL_3);
                    break;

                case 1970 ... 4095: // momentum vector LEFT, LEFT LED ON
                    LED_FR_OFF();
                    LED_FL_ON();

                    // Call led_brake() to indicate slowing down
                    ble_send("left turn\n");
                    // set flag false
                    if (left_turn_flag) {
                        led_brake();
                        right_turn_flag = true;
                        ble_send("**left turn first time\n");
                        left_turn_flag = false;
                    }

                    motor_pwm(PWM_LEVEL_3);
                    break;


                /* Invoke a rebuilt of this part!!
                 * The problem lays in changing from turn to turn. The ADC reads values inside the straight-sections region.
                 * This calls the led_brake() function to often when the speed is not and should not be changed.
                 */
                default:
                    LED_FL_OFF();
                    LED_FR_OFF();

                    // debug msg
                    ble_send("going straight\n");

                    // set flags for led_brake() true
                    right_turn_flag = true;
                    left_turn_flag = true;

                    motor_pwm(PWM_LEVEL_4);
                    break;
                }
                flag_62ms = 0;
            }
            break;
        }

        case STATE_STOPPED:
        {
            #ifdef FSM_STATE_DBG
            if (state_FSM_flag3) {
                ble_send("In STATE_STOPPED\n");
                state_FSM_flag3 = false;
            }
            #endif

            // stop the engine
            motor_brake();

            // blink rear LEDs when in stopped lap
            if (variable_delay_ms(6, 800)) {
                LED_RR_toggle();
                LED_RL_toggle();

//                #ifdef FSM_STATE_DBG
//                // after 30 seconds go back to Ref_lap
//                static uint8_t temp_counter = 0;
//                ble_send("FSM reset in: ");
//                ble_send_uint16(5 - temp_counter);
//                ble_send("\n");
//                temp_counter++;
//
//                if (temp_counter == 5){
//                    state_machine_reset();
//                    temp_counter = 0;
//                }
//                #endif
            }

            // print map
//            static uint8_t temp = 0;
//            if (temp == 0){
//                show_map();
//                temp = 1;
//            }


            /*Set condition to move into next state */

//            state_transition(10); // unknown state -> move to ERROR
            break;
        }

        case STATE_ERROR:
        {
            #ifdef FSM_STATE_DBG
            if (state_FSM_flag4) {
                ble_send("In STATE_ERROR!\n");
                state_FSM_flag4 = false;
            }
            #endif

            // Error handling or recovery
//            state_transition(STATE_STOPPED);
            break;
        }

        case STATE_DEBUG:
        {
            #ifdef FSM_STATE_DBG
            if (state_FSM_flag5) {
                ble_send("In STATE_DEBUG!\n");
                state_FSM_flag5 = false;
            }
            #endif

            motor_pwm(PWM_LEVEL_3);

            if (variable_delay_ms(5, 62)) {

                z_axis = ADC_get_result(4);

                #ifdef FSM_DBG_SEND_ADC
                // Send data over UART BLUETOOTH
                ble_send_uint16(z_axis);
                ble_send("\n");
                #endif

                /* OTHER TASKS HERE */
                if(!save_to_map(z_axis)){      // save to map

                    // print map
                    show_map();

                    // mapIndex overflowed! Change states
                    state_transition(STATE_STOPPED);
                }
            }

//            if (variable_delay_ms(6, 1000)) {
//                ble_send("Global Time ms: \n");
//                ble_send_int32(global_time_ms);
//                ble_send("\n");
//              }

            break;
        }

        default:
        {
            #ifdef FSM_STATE_DBG
            ble_send("\nUnknown State!\n");
            // Error handling or recovery
            #endif
            break;
        }
    }
}

// Function to handle state transitions
void state_transition(State new_state)
{
    #ifdef FSM_STATE_DBG
    ble_send("Transitioning from ");
    ble_send_uint16(current_state);
    ble_send(" to ");
    ble_send_uint16(new_state);
    ble_send(".\n");
    #endif
    current_state = new_state; // go to the next state
}

// Function to restart state machine
void state_machine_reset(void)
{
    // set flags to true
    init_FSM_flag   = true;
    #ifdef FSM_STATE_DBG
    state_FSM_flag1 = true;
    state_FSM_flag2 = true;
    state_FSM_flag3 = true;
    state_FSM_flag4 = true;
    state_FSM_flag5 = true;
    #endif

    corrClearBuffers();     // clear correlation buffers
    mapIndex = 0;       // set map index to 0
    ble_send("State Machine Restarting...\n");
}

/***************************************************************************************************************/
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
/***************************************************************************************************************/
// A function to print laps when called
void lap_counter(void)
{
    static uint8_t lap_counter = 0;
    lap_counter++;
    ble_send("Lap counter: ");
    ble_send_uint16(lap_counter);
    ble_send("\n");
}

/***************************************************************************************************************/
/*
 * A function to create a map of the race track.
 * Returns FALSE is overflows
 */
bool save_to_map(uint16_t adcValue)
{
    static uint16_t distanceFromStart = 0;

    if ((mapIndex < MAP_SAMPLES_LENGTH) && (adcValue != 0)) {
        trackMap[mapIndex].adcValue = adcValue;                 // save the ADC value
        trackMap[mapIndex].timeFromStart = global_time_ms;      // save timestamp in ms

        /* THIS NEED A REPAIR */
        // get distance from start  !!!  CHANGE ACCORDINGLY  !!!
        // Assuming speed is set to PWM_LEVEL_3 = 1 m/s
        // Assuming this function is called every 62ms (please match in the calling if not met!)
            // Assuming mapIndex is incremented every new call - meaning every 62ms
        distanceFromStart = 1 * 62 * mapIndex / 10;          // give the distance in meters so "/10" gives centimeters
        trackMap[mapIndex].distanceFromStart = distanceFromStart; // save distance from start stamp

        // BLE DBG
        #ifdef MAP_DBG
        ble_send("DBG: \tInx \tADC \tDist \ttms \n");
        ble_send(" \t");
        ble_send_uint16(mapIndex);
        ble_send(" \t");
        ble_send_uint16(trackMap[mapIndex].adcValue);
        ble_send(" \t");
        ble_send_uint16(trackMap[mapIndex].distanceFromStart);
        ble_send(" \t");
        ble_send_uint16(trackMap[mapIndex].timeFromStart);
        ble_send("\n");
        #endif

        mapIndex++;     // increment the index counter

        return true;    // return TRUE if mapIndex in range
    }
    return false;       // return false when mapIndex overflows
}

/***************************************************************************************************************/
/*
 * A function to show/print MAP over BLE
 */
void show_map(void)
{

    uint16_t i = 0;

    ble_send("MAP: \tInx \tADC \tDist \ttms \n");
    // print map until 0 is the next value
    while (trackMap[i].adcValue != 0)
    {
        ble_send(" \t");
        ble_send_uint16(i);
        ble_send(" \t");
        ble_send_uint16(trackMap[i].adcValue);
        ble_send(" \t");
        ble_send_uint16(trackMap[i].distanceFromStart);
        ble_send(" \t");
        ble_send_int32(trackMap[i].timeFromStart);
        ble_send("\n");

        i++;
    }

    ble_send("Map Printed!\n");

}

/***************************************************************************************************************/
/*
 * A function to clean the whole map
 */
void dump_map(void)
{
    uint16_t i = 0;
    // Clear all values in the array using a loop
    for (i = 0; i < MAP_SAMPLES_LENGTH; i++) {
        trackMap[i].adcValue = 0;
        trackMap[i].distanceFromStart = 0;
        trackMap[i].timeFromStart = 0;
    }
    ble_send("Map cleared!\n");
}






// **************************************INTERUPTS************************************** //
