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

// DEFINITIONS
#define axis_enable_z                   // set z y x for axis to be enabled for ADC motor control
#define MAP_SAMPLES_LENGTH 100         // Define MAX samples length for the map buffer

    /* DEBUG */
//#define FSM_STATE_DBG             // comment out to disable BLE DBG messages of FSM STATES DEBUG
//#define FSM_DBG                   // comment out to disable BLE DBG messages of FSM DEBUG
//#define FSM_DBG_SEND_ADC          // comment out to disable BLE DBG messages of FSM SEND ADC DEBUG
#define MAP_DBG                   // comment out to disable BLE DBG messages of MAP function

// Define the states for the state machine
typedef enum {
    STATE_REF_LAP,
    STATE_RUNNING,
    STATE_STOPPED,
    STATE_ERROR,
    STATE_DEBUG,
    // Add other states as needed
} State;

// Define map stamps
typedef struct {
    uint16_t adcValue;              // ADC value samled at the time
    uint16_t distanceFromStart;     // Could be distance (cm) or time (ms)
    uint16_t timeFromStart;         // Time from start. !!! CONSIDER LONGER TYPE IF THE TRACK IS LONGER THAN 64 sec !!!
} MapPoint;

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

#endif /* AUX_H_ */
