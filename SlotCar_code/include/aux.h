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

// DEFINITIONS
#define axis_enable_z                   // set z y x for axis to be enabled for ADC motor control
#define MAP_SAMPLES_LENGTH 400          // Define MAX samples length for the map buffer
#define TRANSITION_PERCENTAGE 25        // Speed Control Constants
#define BEND_SEGMENT 1
#define STRAIGHT_SEGMENT 0

    /* DEBUG */
#define FSM_STATE_DBG             // comment out to disable BLE DBG messages of FSM STATES DEBUG
#define FSM_DBG                   // comment out to disable BLE DBG messages of FSM DEBUG
//#define FSM_DBG_SEND_ADC          // comment out to disable BLE DBG messages of FSM SEND ADC DEBUG
//#define MAP_DBG                   // comment out to disable BLE DBG messages of MAP function
#define SPEED_DBG

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
    uint16_t distanceFromStart;     // Could be distance (cm) or time (ms)
    uint32_t timeFromStart;         // Time from start. !!! CONSIDER LONGER TYPE IF THE TRACK IS LONGER THAN 64 sec !!!
} MapPoint;

// Define map segments
typedef struct {
    uint8_t segmentIndex;               // index of a section. Consider longer types for tracks with more than 255 segments
    uint8_t segmentType;                // type of a section; 0 = straight, 1 = right turn, 2 = left turn
    uint16_t segmentLength;             // actual length of a section
    uint16_t segmentTime;               // actual time of a section
    uint16_t segmentDistanceFromStart;
}MapSegment;

// VARIABLES

// FUNCTIONS
void car_control_simple(void);          // simple motor PWM ADC data controlling
void state_machine_init(void);          // Initialize the state machine
void car_control_FSM(void);             // a Finite State Machine for motor PWM controlling
void state_transition(State new_state); // Transition between states
void state_machine_reset(void);         // Reset State Machine
uint16_t feed_stored_data(const uint16_t *storedData, uint16_t dataLength); // goes thru saved data and feeds it as an output
void lap_counter(void);                 // when called it prints counted laps
bool save_to_map(uint16_t adcValue);    // save to map
void show_map(void);                    // show map over BLE
void dump_map(void);                    // delete all map samples
void create_map(void);
pwm_level_t adjust_speed(uint16_t currentDistance);
uint8_t get_current_segment(uint16_t currentDistance);
pwm_level_t get_speed_mps_10(pwm_level_t pwm_level);

#endif /* AUX_H_ */
