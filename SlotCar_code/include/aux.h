/*
 * aux.h
 *
 *  Created on: 22/October/2024
 *      Author: xkosik09
 */

#ifndef AUX_H_
#define AUX_H_

// INCLUDES
#include <msp430.h>
#include "include/motor.h"
#include <stdbool.h>

// DEFINITIONS
#define axis_enable_z                   // set z y x for axis to be enabled for ADC motor control
#define MAP_SAMPLES_LENGTH 1000         // Define MAX samples length for the map buffer
#define TRANSITION_PERCENTAGE 25        // Speed Control Constants
#define BEND_SEGMENT 1                  // define bend segment in map
#define STRAIGHT_SEGMENT 0              // define straight segment in map
#define CONFIRMING_SAMPLES_COUNT 2      // define a number of needed samples to start a new segment
#define LOWER_STRAIGHT_RANGE 1960       // define lower range for a straight section ADC readings. Engine must be running to match this range!
#define UPPER_STRAIGHT_RANGE 1968       // define upper range for a straight section ADC readings. Engine must be running to match this range!
#define MAX_LENGTH_REF_LAP 2000          // set max length for REF_LAP in cm. After overflow FSM changes states
#define MAX_NUMBER_OF_LAPS_IN_RACE 5    // set max number of laps during the race


    /* DEBUG */
#define FSM_STATE_DBG             // comment out to disable BLE DBG messages of FSM STATES DEBUG
//#define FSM_DBG                   // comment out to disable BLE DBG messages of FSM DEBUG
//#define FSM_DBG_SEND_ADC          // comment out to disable BLE DBG messages of FSM SEND ADC DEBUG
//#define MAP_DBG                   // comment out to disable BLE DBG messages of MAP function
//#define CREATE_MAP_DBG            // comment out to disable BLE DBG messages of MAP function
#define SPEED_DBG                 // comment out to disable BLE DBG messages of SPEED adjusts
#define BLE_DBG_MSGS              // comment out to disable BLE DBG messages of general usage

// Define the states for the state machine
typedef enum {
    STATE_REF_LAP,
    STATE_RUNNING,
    STATE_SMART_RUNNING,
    STATE_STOPPED,
    STATE_ERROR,
    STATE_DEBUG,
    // Add other states as needed
} State;

// Define map stamps
typedef struct {
    uint16_t adcValue;              // ADC value sampled at the time
    uint32_t distanceFromStart;     // Could be distance (cm) or time (ms)
    uint16_t timeFromStart;         // Time from start. !!! CONSIDER LONGER TYPE IF THE TRACK IS LONGER THAN 64 sec !!!
} MapPoint;

// Define map segments
typedef struct {
    uint8_t segmentIndex;               // index of a section. Consider longer types for tracks with more than 255 segments
    uint8_t segmentType;                // type of a section; 0 = straight, 1 = right turn, 2 = left turn
    uint32_t segmentLength;             // actual length of a section. mMltiply by 1000 to get in cm.
    uint16_t segmentTime;               // actual time of a section
    uint32_t segmentDistanceFromStart;  // Distance from start of the lap. Multiply by 1000 to get in cm.
}MapSegment;

// VARIABLES
extern volatile bool reset_flag;        // reset flag
extern uint8_t segmentsCount;           // counter for segments

// FUNCTIONS
void car_control_simple(void);          // simple motor PWM ADC data controlling
void state_machine_init(void);          // Initialize the state machine
void car_control_FSM(void);             // a Finite State Machine for motor PWM controlling
void state_transition(State new_state); // Transition between states
void state_machine_reset(void);         // Reset State Machine
uint16_t feed_stored_data(const uint16_t *storedData, uint16_t dataLength); // goes thru saved data and feeds it as an output
uint8_t lap_counter(void);              // when called it prints counted laps. Returns the lap number.
bool save_to_map(uint16_t adcValue);    // save to map
void show_map(void);                    // show map over BLE
void dump_map(void);                    // delete all map samples
uint8_t create_map(void);               // function to create map from ADC samples
void show_map_segments(void);           // show map over BLE
pwm_level_t adjust_speed(uint32_t currentDistance, uint16_t z_axis);     // adjust speed of the car
uint8_t get_current_segment(uint32_t currentDistance);  // goes thru segments of the map and returns the current segment
pwm_level_t get_speed_mps_10(pwm_level_t pwm_level);    // returns speed in m/s
void smart_car_leds(uint16_t z_axis);              // controls LEDs of the car with the real-time ADC values

#endif /* AUX_H_ */
