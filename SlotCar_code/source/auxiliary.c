/*
 * auxiliary.c
 *
 *  Created on: 22/October/2024
 *      Author: xkosik09
 */

// INCLUDES
#include <include/auxiliary.h>
#include <msp430.h>
#include "include/timers.h"
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
volatile bool reset_flag = false;
#ifdef FSM_STATE_DBG
static bool state_FSM_flag1= true;
static bool state_FSM_flag2= true;
static bool state_FSM_flag3= true;
static bool state_FSM_flag4= true;
static bool state_FSM_flag5= true;
static bool state_FSM_flag6= true;
#endif

static uint16_t savedMapIndex = 0;       // represents max valid samples in saved map data
const uint16_t temp_total_duration = 0;
uint16_t mapTotalDuration = 0;
uint32_t mapTotalLength = 0;            // Total map length. Divide by 1000 to get in cm
uint32_t mapCurrentDistance = 0;        // Current map distance. Divide by 1000 to get in cm
uint8_t segmentsCount = 0;              // holds number of segments

// Define map length
MapPoint trackMap[MAP_SAMPLES_LENGTH];              // Example for 1000 samples max
MapSegment mapSegments[MAP_SAMPLES_LENGTH / 4];     // Max map segments

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
    state_FSM_flag6 = true;
    #endif

    LED_FR_OFF();               // switch LEDs off
    LED_FL_OFF();               // switch LEDs off
    LED_RL_OFF();               // switch LEDs off
    LED_RL_OFF();               // switch LEDs off

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

    // check for reset state
    if (reset_flag) {

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

            // set constant speed for reference lap // CHECK TO SET THE SAME ON THE WHOLE FOLLOWING CODE
            motor_pwm(PWM_LEVEL_3); // means ~1 m/s

            // blink rear LEDs while in reference lap
            if (variable_delay_ms(1, 300)) {
                LED_RR_toggle();
                LED_RL_toggle();
            }

            // perform correlation every xx ms
            // use 10ms for stored data
            // test usage of 62ms for ADC to match the speed control
                // if CORRELATION_WINDOW=100, then this takes samples of 5 seconds of the track
            if (variable_delay_ms(2, 62)) {
                z_axis = ADC_get_result(4);     // get ADC results
                // map distance counter. 60 represents speed PWM_LEVEL_3 ~0.6 m/s
                mapCurrentDistance += (uint32_t)60 * 62; // 62 is the variable delay in ms

                // Use stupid LEDs
                switch(z_axis) {
                    case 0 ... 1959:    // momentum vector RIGHT, RIGHT LED ON
                    {
                        LED_FR_ON();    // control LEDs
                        LED_FL_OFF();   // control LEDs
                        break;
                    }
                    case 1970 ... 4095: // momentum vector LEFT, LEFT LED ON
                    {
                        LED_FL_ON();    // control LEDs
                        LED_FR_OFF();    // control LEDs
                        break;
                    }
                    default:
                    {
                        LED_FL_OFF();
                        LED_FR_OFF();
                    }
                }

                #ifdef FSM_DBG_SEND_ADC
                // Send data over UART BLUETOOTH
                ble_send_uint16(z_axis);
                ble_send("\n");
                #endif
                #ifdef FSM_DBG
                uint16_t temp_adc_data = feed_stored_data(stored_track_data_1, STORED_DATA_1_LENGTH);
                save_to_map(temp_adc_data);      // save to map
                if (corrDetectNewLapStart(temp_adc_data)) {
                #endif
                #ifndef FSM_DBG
                save_to_map(z_axis);      // save to map
                if (corrDetectNewLapStart(z_axis)) {
                #endif
                    #ifdef FSM_STATE_DBG
                    ble_send("Correlation found at ");
                    ble_send_uint16(mapCurrentDistance / 1000);
                    ble_send(" cm\n");
                    #endif

                    //reset LEDs to OFF state
                    LED_RR_OFF();
                    LED_RL_OFF();

                    segmentsCount = create_map();   // create map
                    mapCurrentDistance = 0;         // set distance to 0
                                // consider setting to something else for better synchronization

                    // show map segments
//                    show_map_segments();
                    #ifdef FSM_DBG
                    show_map_segments();
                    #endif

                    // go to the next state
//                    state_transition(STATE_STOPPED);
                    state_transition(STATE_SMART_RUNNING);
//                    state_transition(STATE_RUNNING);
                }
                // Emergency checker if correlation not found
                if (mapCurrentDistance / 1000 >= MAX_LENGTH_REF_LAP) {
                    ble_send("Correlation not found after reaching max length ");
                    ble_send_uint16(MAX_LENGTH_REF_LAP);
                    ble_send(" cm! \nStarting emergency FSM state...\n");
                    state_transition(STATE_RUNNING);
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
            if (variable_delay_ms(2, 20)) {
                #ifdef FSM_DBG
                z_axis = feed_stored_data(stored_track_data_1, STORED_DATA_1_LENGTH);
                #endif
                #ifndef FSM_DBG
                z_axis = ADC_get_result(4);
                #endif

                switch(z_axis)
                {
                case 0 ... LOWER_STRAIGHT_RANGE - 2:    // momentum vector RIGHT, RIGHT LED ON
                {
                    LED_FR_ON();    // control LEDs
                    LED_FL_OFF();   // control LEDs

                    // Call led_brake() to indicate slowing down
                    if (right_turn_flag) {
                        led_brake();
                        motor_brake(BRAKE_LEVEL_2); // brake
                        left_turn_flag = true;      // set flag true
                        right_turn_flag = false;    // set flag false
                    }

                    // check for brakes and change speed accordingly
                    if (!flag_braking_in_progress) {
                        // change the speed of the motor
                        motor_pwm(PWM_LEVEL_4);
                    } else {
//                        ble_send("Braking in process...\n");
                    }
                    break;
                }
                case UPPER_STRAIGHT_RANGE + 2 ... 4095: // momentum vector LEFT, LEFT LED ON
                {
                    LED_FL_ON();    // control LEDs
                    LED_FR_OFF();   // control LEDs

                    // Call led_brake() to indicate slowing down
                    if (left_turn_flag) {
                        led_brake();
                        motor_brake(BRAKE_LEVEL_2); // brake
                        right_turn_flag = true;     // set flag true
                        left_turn_flag = false;     // set flag false
                    }

                    // check for brakes and change speed accordingly
                    if (!flag_braking_in_progress) {
                        // change the speed of the motor
                        motor_pwm(PWM_LEVEL_4);
                    } else {
//                        ble_send("Braking in process...\n");
                    }
                    break;
                }

                /* Invoke a rebuilt of this part!!
                 * The problem lays in changing from turn to turn. The ADC reads values inside the straight-sections region.
                 * This calls the led_brake() function to often when the speed is not and should not be changed.
                 */
                default:
                {
                    LED_FL_OFF();
                    LED_FR_OFF();

                    // set flags for led_brake() true
                    right_turn_flag = true;
                    left_turn_flag = true;

                    // check for brakes and change speed accordingly
                    if (!flag_braking_in_progress) {
                        // change the speed of the motor
                        motor_pwm(PWM_LEVEL_5);
                    } else {
                        ble_send("Braking in process...\n");
                    }

                    break;
                }
                }
            }
            break;
        }

        /*
         * This is the main state where the car speed is controlled.
         */
        case STATE_SMART_RUNNING:
        {
            #ifdef FSM_STATE_DBG
            if (state_FSM_flag6) {
                ble_send("In STATE_SMART_RUNNING\n");
                state_FSM_flag6 = false;
            }
            #endif

            /* SMART CODE HERE */
            static uint8_t laps = 0;
            static pwm_level_t actual_speed = PWM_LEVEL_3;  // initial speed at the start
            static uint16_t actual_speed_mps = 0;

            actual_speed_mps = get_speed_mps_10(actual_speed); // get actual speed

            if (variable_delay_ms(2, 62)) {
                // get ADC values either stored or real-time
                #ifdef FSM_DBG
                z_axis = feed_stored_data(stored_track_data_1, STORED_DATA_1_LENGTH);
                #endif
                #ifndef FSM_DBG
                z_axis = ADC_get_result(4);
                #endif

                mapCurrentDistance += (uint32_t)actual_speed_mps * 62;      // 62 is the variable delay in ms // get mapCurrentDistance
                actual_speed = adjust_speed(mapCurrentDistance, z_axis);    // smart speed control
                smart_car_leds(z_axis);                                     // smart LEDs control

                // set condition when the lap ends
                if (mapCurrentDistance >= mapTotalLength) {
                    mapCurrentDistance -= mapTotalLength;   // wrap mapCurrentDistance around and continue
                    laps = lap_counter();                   // call lap counter
                } else if (reset_flag) {
                    laps = lap_counter();                   // reset lap counter
                }
                //Set condition to move into the next state (e.g., end of the race)
                if (laps >= MAX_NUMBER_OF_LAPS_IN_RACE) {
                    state_transition(STATE_STOPPED);
                    LED_FR_OFF();               // switch LEDs off
                    LED_FL_OFF();               // switch LEDs off
                    LED_RL_OFF();               // switch LEDs off
                    LED_RL_OFF();               // switch LEDs off
                    ble_send("Race Finished!\n");
                }
            }

            /*Set condition to move into next state */
//            state_transition(10); // unknown state -> move to ERROR
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
            motor_brake(BRAKE_LEVEL_INF);

            // blink LEDs when in stopped lap
            if (variable_delay_ms(1, 800)) {
                LED_RR_toggle();    // toggle LEDs
                LED_RL_toggle();    // toggle LEDs
                LED_FR_toggle();    // toggle LEDs
                LED_FL_toggle();    // toggle LEDs
            }

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
            // after 30 seconds go back to Ref_lap
            static uint8_t temp_counter = 0;
            ble_send("FSM reset in: ");
            ble_send_uint16(5 - temp_counter);
            ble_send("\n");
            temp_counter++;

            // reset FSM
            if (temp_counter == 5){
                state_machine_reset();
                temp_counter = 0;
            }

            /* OTHER TASKS HERE */


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

                save_to_map(z_axis);

                /* OTHER TASKS HERE */
//                if(!save_to_map(z_axis)){      // save to map
//
//                    // print map
//                    show_map();
//
//                    // savedMapIndex overflowed! Change states
//                    state_transition(STATE_STOPPED);
//                }
            }
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

/***************************************************************************************************************/
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

/***************************************************************************************************************/
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
    state_FSM_flag6 = true;
    #endif

    corrClearBuffers();         // clear correlation buffers
    savedMapIndex = 0;          // set map index to 0
    mapCurrentDistance = 0;     // reset mapCurrentDistance

    LED_FR_OFF();               // switch LEDs off
    LED_FL_OFF();               // switch LEDs off
    LED_RL_OFF();               // switch LEDs off
    LED_RL_OFF();               // switch LEDs off
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
uint8_t lap_counter(void)
{
    static uint8_t lap_counter = 0;
    if (reset_flag) {
        lap_counter = 0;  // Reset the counter
        reset_flag = false;  // Clear the flag
        ble_send("Lap counter cleared!\n");
    } else {
        lap_counter++;
        ble_send("Lap counter: ");
        ble_send_uint16(lap_counter);
        ble_send("\n");
    }
    return lap_counter; // Return current lap count
}

/***************************************************************************************************************/
/*
 * A function to create a map of the race track.
 * Returns FALSE if overflows
 */
bool save_to_map(uint16_t adcValue)
{
    static uint32_t distanceFromStart = 0;

    // Checker for dumping the first data samples
    static uint16_t dump_counter = 0;       // First dump data counter
    if (dump_counter < DUMP_SAMPLES) {
        dump_counter++;       // increment counter
        return false;           // return false to ignore DUMP_SAMPLES samples
    }

    if ((savedMapIndex < MAP_SAMPLES_LENGTH) && (adcValue != 0)) {
        trackMap[savedMapIndex].adcValue = adcValue;                 // save the ADC value
        trackMap[savedMapIndex].timeFromStart = global_time_ms;      // save timestamp in ms

        /* THIS NEEDS A REPAIR */
        // get distance from start  !!!  CHANGE ACCORDINGLY  !!!
        // Assuming speed is set to PWM_LEVEL_3 = 0.6 m/s (calculated 25/Nov/2024 (please provide the data))
        // Assuming this function is called every 62ms (please match in the calling if not met!)
            // Assuming savedMapIndex is incremented every new call - meaning every 62ms
        // Let's assume the distance here is only for the code. It does not need to represent real distance.
        distanceFromStart = (uint32_t)60 * 62 * savedMapIndex;          // give the distance in meters so "/100" gives millimeters
        trackMap[savedMapIndex].distanceFromStart = distanceFromStart; // save distance from start stamp

        // BLE DBG
        #ifdef MAP_DBG
        ble_send("DBG: \tInx \tADC \tDist \ttms \n");
        ble_send(" \t");
        ble_send_uint16(savedMapIndex);
        ble_send(" \t");
        ble_send_uint16(trackMap[savedMapIndex].adcValue);
        ble_send(" \t");
        ble_send_uint16(trackMap[savedMapIndex].distanceFromStart / 1000);
        ble_send(" \t");
        ble_send_int32(trackMap[savedMapIndex].timeFromStart);
        ble_send("\n");
        #endif

        savedMapIndex++;     // increment the index counter

        return true;    // return TRUE if savedMapIndex in range
    }
    ble_send("ERROR: Map storage full!\n");
    return false;       // return false when savedMapIndex overflows
}

/***************************************************************************************************************/
/*
 * A function to show/print MAP over BLE
 */
void show_map(void)
{
    uint16_t i = 0;
    ble_send("MAP: \tInx \tADC \tDist \ttms \n");
    // print map until savedMapIndex, which represents valid samples saved in the map
    for (i = 0; i < savedMapIndex; i++)
    {
        ble_send(" \t");
        ble_send_uint16(i);
        ble_send(" \t");
        ble_send_uint16(trackMap[i].adcValue);
        ble_send(" \t");
        ble_send_uint16(trackMap[i].distanceFromStart / 1000);
        ble_send(" \t");
        ble_send_int32(trackMap[i].timeFromStart);
        ble_send("\n");
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
    for (i = 0; i < savedMapIndex; i++) {
        trackMap[i].adcValue = 0;
        trackMap[i].distanceFromStart = 0;
        trackMap[i].timeFromStart = 0;
    }
    ble_send("Map cleared!\n");
}

/***************************************************************************************************************/
/*
 * A function to determine what the saved map is and to create segments from
 */
uint8_t create_map(void)
{
    uint16_t i = 0;
    uint8_t ii = 0;
    mapTotalDuration = 0;
    mapTotalLength = 0;
    uint16_t segmentStartIndex = 0;
    uint8_t currentSegmentType = (trackMap[0].adcValue >= LOWER_STRAIGHT_RANGE && trackMap[0].adcValue <= UPPER_STRAIGHT_RANGE) ? STRAIGHT_SEGMENT : BEND_SEGMENT;
    uint16_t confirmingSampleCount = 0; // To track the number of confirming samples for transition
    uint16_t segmentCount = 0;

    for (i = 1; i < savedMapIndex; i++) {
        // Determine the type of the current point
        uint8_t newSegmentType = (trackMap[i].adcValue >= LOWER_STRAIGHT_RANGE && trackMap[i].adcValue <= UPPER_STRAIGHT_RANGE) ? STRAIGHT_SEGMENT : BEND_SEGMENT;

        // Check if the type matches the current segment type
        if (newSegmentType == currentSegmentType) {
            confirmingSampleCount = 0; // Reset confirming samples
        } else {
            confirmingSampleCount++; // Increment confirming samples for transition
        }

        // If confirming samples reach threshold, finalize the current segment
        if (confirmingSampleCount >= CONFIRMING_SAMPLES_COUNT) {
            // Add the confirming samples to the correct segment
            if (currentSegmentType == newSegmentType) {
                // If the new samples match the current type, include them in the current segment
                confirmingSampleCount = 0; // Reset the counter as no transition occurs
            } else {
                // Finalize the current segment
                mapSegments[segmentCount].segmentIndex = segmentCount;
                mapSegments[segmentCount].segmentType = currentSegmentType;
                mapSegments[segmentCount].segmentLength =
                        trackMap[i - confirmingSampleCount].distanceFromStart -
                        trackMap[segmentStartIndex].distanceFromStart;
                mapSegments[segmentCount].segmentTime =
                        trackMap[i - confirmingSampleCount].timeFromStart -
                        trackMap[segmentStartIndex].timeFromStart;

                // Set the segment's start distance from the start of the lap
                mapSegments[segmentCount].segmentDistanceFromStart = trackMap[segmentStartIndex].distanceFromStart;

                segmentCount++;

                // Start a new segment with the confirming samples as the beginning
                segmentStartIndex = i - confirmingSampleCount;
                currentSegmentType = newSegmentType;
                confirmingSampleCount = 0;
            }
        }
    }

    // Finalize the last segment
    mapSegments[segmentCount].segmentIndex = segmentCount;
    mapSegments[segmentCount].segmentType = currentSegmentType;
    mapSegments[segmentCount].segmentLength = trackMap[i - 1].distanceFromStart - trackMap[segmentStartIndex].distanceFromStart;
    mapSegments[segmentCount].segmentTime = trackMap[i - 1].timeFromStart - trackMap[segmentStartIndex].timeFromStart;

    // Set the segment's start distance from the start of the lap
    mapSegments[segmentCount].segmentDistanceFromStart = trackMap[segmentStartIndex].distanceFromStart;

    segmentCount++;

    // BLE DBG
    #ifdef MAP_DBG
    ble_send("segmentCount: ");
    ble_send_uint16(segmentCount);
    ble_send("\n");
    #endif

    // check for invalid samples
    if (savedMapIndex > MAP_SAMPLES_LENGTH) {
        ble_send("ERROR: Map segments storage full!\n");
    }
    else if (trackMap[0].adcValue < ADC_LOWER_FRAME && trackMap[0].adcValue > ADC_UPPER_FRAME) { // defined in ADC.h
        ble_send("ERROR: Unknown Map Segment!\n");
    }

    // get mapTotalDuration and mapTotalLength
    for (ii = 0; ii < segmentCount; ii++ ) {
        mapTotalDuration += mapSegments[ii].segmentTime;
        mapTotalLength   += mapSegments[ii].segmentLength;
    }
    ble_send("Map segmentation completed.\n");

    return segmentCount; // returns number of segments (number + 1)
}
/***************************************************************************************************************/
/*
 * Function to show segments of the map
 */
void show_map_segments(void)
{
    uint8_t i = 0;

    ble_send("MAP SEGMENTS:\n \tIndex \tType \tLength \tDur \tdistanceFromStart \n");
    for (i = 0; i < segmentsCount; i++ ) {
        ble_send(" \t");
        ble_send_uint16(mapSegments[i].segmentIndex);
        ble_send(" \t");
        ble_send_uint16(mapSegments[i].segmentType);
        ble_send(" \t");
        ble_send_uint16(mapSegments[i].segmentLength / 1000);
        ble_send(" \t");
        ble_send_uint16(mapSegments[i].segmentTime);
        ble_send(" \t");
        ble_send_uint16(mapSegments[i].segmentDistanceFromStart / 1000);
        ble_send("\n");
    }
    ble_send("Total duration: ");
    ble_send_uint16(mapTotalDuration);
    ble_send(" ms \nTotal length: ");
    ble_send_uint16(mapTotalLength / 1000);
    ble_send(" cm \n");
}
/***************************************************************************************************************/
/*
 * Speed control function
 */
pwm_level_t adjust_speed(uint32_t currentDistance, uint16_t z_axis)
{
    static pwm_level_t currentSpeed = PWM_LEVEL_4;  // Default speed for reference lap
    uint8_t currentSegment = get_current_segment(currentDistance);
    uint8_t nextSegment = currentSegment + 1;
    uint32_t distanceToEnd = 0;

    // BLE DBG
    #ifdef SPEED_DBG
    ble_send("\ncurrentSegment: ");
    ble_send_uint16(currentSegment);
    ble_send("\t");
    ble_send("nextSegment: ");
    ble_send_uint16(nextSegment);
    ble_send("\n");
    #endif

    // check if the next segment is not outside of range of all segments. // Add condition when segmentsCount only 1
    if (nextSegment >= segmentsCount) {
        distanceToEnd = mapSegments[segmentsCount - 1].segmentDistanceFromStart + mapSegments[segmentsCount - 1].segmentLength - currentDistance;
        nextSegment = 0; // set next segment as the first one in the lap map -> wrap around and go again as the race continuous
    // check if the currentDistance is not exceeding the max length of a lap. If yes, set distanceToEnd to avoid wrapping around
    } else if ((mapSegments[segmentsCount - 1].segmentDistanceFromStart + mapSegments[segmentsCount - 1].segmentLength) <= currentDistance ) {
        distanceToEnd = 0;  // recovery from overflow
    }
    else { // if segments are valid, continue
        distanceToEnd = mapSegments[nextSegment].segmentDistanceFromStart - currentDistance;
    }

    // get transitionDistance to react in time
    uint32_t transitionDistance = (mapSegments[currentSegment].segmentLength * TRANSITION_PERCENTAGE) / 100;

    if (distanceToEnd <= transitionDistance) {
        // check the next segment
        if (mapSegments[nextSegment].segmentType == BEND_SEGMENT && currentSpeed < PWM_LEVEL_4) {
            // Brake ISR to prevent over-steering
            switch (currentSpeed) {
               case PWM_LEVEL_4: motor_brake(BRAKE_LEVEL_2); break;
               case PWM_LEVEL_5: motor_brake(BRAKE_LEVEL_3); break;
               case PWM_LEVEL_6: motor_brake(BRAKE_LEVEL_4); break;
               case PWM_LEVEL_7: motor_brake(BRAKE_LEVEL_5); break;
               case PWM_LEVEL_8: motor_brake(BRAKE_LEVEL_6); break;
               case PWM_LEVEL_9: motor_brake(BRAKE_LEVEL_7); break;
               default: break;
           }
            currentSpeed = PWM_LEVEL_4;

        // condition to speed up by one when leaving a bend
        } else if (mapSegments[nextSegment].segmentType == STRAIGHT_SEGMENT && currentSpeed >= PWM_LEVEL_4) {
            currentSpeed = PWM_LEVEL_5;
            // Emergency override if real-time ADC value indicates a danger on the track in case of improper synchronization
            if (z_axis < LOWER_STRAIGHT_RANGE - 3) {
                if (currentSpeed < PWM_LEVEL_5) {
                    motor_brake(BRAKE_LEVEL_2);
    //            ble_send("emergency!\n");
                } else if (currentSpeed < PWM_LEVEL_6) {
                    motor_brake(BRAKE_LEVEL_4);
                }
                else {
                    motor_brake(BRAKE_LEVEL_1);
        //            ble_send("emergency!\n");
                }
                currentSpeed = PWM_LEVEL_5; // set normal speed
            } else if (z_axis > UPPER_STRAIGHT_RANGE + 0) {
                if (currentSpeed < PWM_LEVEL_5) {
                    motor_brake(BRAKE_LEVEL_2);
    //            ble_send("emergency!\n");
                } else if (currentSpeed < PWM_LEVEL_6) {
                    motor_brake(BRAKE_LEVEL_3);
                } else {
                    motor_brake(BRAKE_LEVEL_1);
        //            ble_send("emergency!\n");
                }
                currentSpeed = PWM_LEVEL_5; // set normal speed
            }
        }
    } else if ((mapSegments[currentSegment].segmentType == STRAIGHT_SEGMENT) && (currentSpeed <= PWM_LEVEL_4)) {
        // very smart condition - it gradually speeds up!
        switch (currentSpeed) {
           case PWM_LEVEL_3: currentSpeed = PWM_LEVEL_4; break;
           case PWM_LEVEL_4: currentSpeed = PWM_LEVEL_5; break;
           // comment out to enable higher speeds
           case PWM_LEVEL_5: currentSpeed = PWM_LEVEL_6; break;
//           case PWM_LEVEL_6: currentSpeed = PWM_LEVEL_7; break;
//           case PWM_LEVEL_7: currentSpeed = PWM_LEVEL_8; break;
//           case PWM_LEVEL_8: currentSpeed = PWM_LEVEL_9; break;
//           case PWM_LEVEL_9: currentSpeed = PWM_LEVEL_10; break;
           default: currentSpeed = PWM_LEVEL_5; break;
       }
        // Emergency override if real-time ADC value indicates a danger on the track in case of improper synchronization
        if (z_axis < LOWER_STRAIGHT_RANGE - 3) {
            if (currentSpeed < PWM_LEVEL_5) {
                motor_brake(BRAKE_LEVEL_2);
//            ble_send("emergency!\n");
            } else if (currentSpeed < PWM_LEVEL_6) {
                motor_brake(BRAKE_LEVEL_3);
            }
            else {
                motor_brake(BRAKE_LEVEL_1);
    //            ble_send("emergency!\n");
            }
            currentSpeed = PWM_LEVEL_5; // set normal speed
        } else if (z_axis > UPPER_STRAIGHT_RANGE + 0) {
            if (currentSpeed < PWM_LEVEL_5) {
                motor_brake(BRAKE_LEVEL_2);
//            ble_send("emergency!\n");
            } else if (currentSpeed < PWM_LEVEL_6) {
                motor_brake(BRAKE_LEVEL_3);
            } else {
                motor_brake(BRAKE_LEVEL_1);
    //            ble_send("emergency!\n");
            }
            currentSpeed = PWM_LEVEL_5; // set normal speed
        }
    } else if (mapSegments[currentSegment].segmentType == BEND_SEGMENT && currentSpeed < PWM_LEVEL_4) {
        // Brake ISR to prevent over-steering
        switch (currentSpeed) {
           case PWM_LEVEL_4: motor_brake(BRAKE_LEVEL_1); break;
           case PWM_LEVEL_5: motor_brake(BRAKE_LEVEL_2); break;
           case PWM_LEVEL_6: motor_brake(BRAKE_LEVEL_3); break;
           case PWM_LEVEL_7: motor_brake(BRAKE_LEVEL_4); break;
           case PWM_LEVEL_8: motor_brake(BRAKE_LEVEL_5); break;
           case PWM_LEVEL_9: motor_brake(BRAKE_LEVEL_6); break;
           default: break;
       }
        currentSpeed = PWM_LEVEL_4;
    }

    /* !!!! Add condition to prevent speeding too much */
    /* !!!! Add condition to check ADC real-time samples to ensure better synchronization */

    // check if braking is in process
    if (!flag_braking_in_progress) {
        // change the speed of the motor
        motor_pwm(currentSpeed);
    } else {
        ble_send("Braking in process...\n");
    }


    // BLE DBG
    #ifdef SPEED_DBG
    ble_send("Current map segment: ");
    ble_send(" \t");
    ble_send_uint16(mapSegments[currentSegment].segmentIndex);
    ble_send(" \t");
    ble_send_uint16(mapSegments[currentSegment].segmentType);
    ble_send(" \t");
    ble_send_uint16(mapSegments[currentSegment].segmentLength / 1000);
    ble_send(" \t");
    ble_send_uint16(mapSegments[currentSegment].segmentTime);
    ble_send("\n");
    ble_send("Current speed: ");
    ble_send_uint16(currentSpeed);
    ble_send(" PWM;  ");
    ble_send_uint16(get_speed_mps_10(currentSpeed));
    ble_send(" cm/s\n");
    ble_send("currentDistance: ");
    ble_send_uint16(currentDistance / 1000);
    ble_send("\n");
    ble_send("distanceToEnd: ");
    ble_send_uint16(distanceToEnd / 1000);
    ble_send("\n");
    ble_send("transitionDistance: ");
    ble_send_uint16(transitionDistance / 1000);
    ble_send("\n");
    #endif

    return currentSpeed;
}

/***************************************************************************************************************/
/*
 * This function returns current segment from the map where the car is at the time
 */
uint8_t get_current_segment(uint32_t currentDistance)
{
    uint8_t i = 0;
    for (i = 0; i < segmentsCount; i++) {
        // Ensure we're within the valid segment range (no need to check segmentLength == 0 or segmentTime == 0 here)
        if (currentDistance >= mapSegments[i].segmentDistanceFromStart &&
            currentDistance < (mapSegments[i].segmentDistanceFromStart + mapSegments[i].segmentLength)) {
            return i; // Return the index of the current segment
        }
    }
    return 0;  // Default to first segment if no match found
}

/***************************************************************************************************************/
/*
 * This function returns speed in m/s.
 */
uint16_t get_speed_mps_10(pwm_level_t pwm_level)
{
    switch(pwm_level)
    {
        case PWM_LEVEL_1:  return 25;   // 0.25 m/s * 10 = 2.5
        case PWM_LEVEL_2:  return 28;   // 0.28 m/s * 10 = 2.8
        case PWM_LEVEL_3:  return 60;   // 0.6 m/s * 10 = 6
        case PWM_LEVEL_4:  return 75;   // 0.75 m/s * 10 = 7.5
        case PWM_LEVEL_5:  return 100;  // 1.0 m/s * 10 = 10
        case PWM_LEVEL_6:  return 120;  // 1.2 m/s * 10 = 12
        case PWM_LEVEL_7:  return 125;  // 1.25 m/s * 10 = 12.5
        case PWM_LEVEL_8:  return 140;  // 1.4 m/s * 10 = 14
        case PWM_LEVEL_9:  return 180;  // 1.8 m/s * 10 = 18
        case PWM_LEVEL_10: return 200;  // 2.0 m/s * 10 = 20
        default:           return 0;    // Return 0 if invalid PWM level
    }
}

/***************************************************************************************************************/
/*
 * This functions controls LEDs according to the real-time ADC data
 */
void smart_car_leds(uint16_t z_axis)
{
    // set flags for turns
    static bool right_turn_flag = true;
    static bool left_turn_flag = true;

    switch(z_axis) {
        case 0 ... 1959:    // momentum vector RIGHT, RIGHT LED ON
        {
            LED_FR_ON();    // control LEDs
            LED_FL_OFF();   // control LEDs

            // Call led_brake() to indicate slowing down
            if (right_turn_flag) {
                led_brake();
                left_turn_flag = true;      // set flag true
                right_turn_flag = false;    // set flag false
            }
            break;
        }
        case 1970 ... 4095: // momentum vector LEFT, LEFT LED ON
        {
            LED_FL_ON();    // control LEDs
            LED_FR_OFF();    // control LEDs

            // Call led_brake() to indicate slowing down

            if (left_turn_flag) {
                led_brake();
                right_turn_flag = true;     // set flag true
                left_turn_flag = false;     // set flag false
            }
            break;
        }
        /* Invoke a rebuilt of this part!!
         * The problem lays in changing from turn to turn. The ADC reads values inside the straight-sections region.
         * This calls the led_brake() function to often when the speed is not and should not be changed.
         */
        default:
        {
            LED_FL_OFF();
            LED_FR_OFF();

            // set flags for led_brake() true
            right_turn_flag = true;
            left_turn_flag = true;
            break;
        }
    }
}











// **************************************INTERUPTS************************************** //
