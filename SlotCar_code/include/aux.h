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
#define axis_enable_z // set z y x for axis to be enabled for ADC motor control
//static bool init_FSM_flag;

    /* DEBUG */
#define FSM_STATE_DBG             // comment out to disable BLE DBG messages of FSM STATES DEBUG
#define FSM_DBG                   // comment out to disable BLE DBG messages of FSM DEBUG
//#define FSM_DBG_SEND_ADC          // comment out to disable BLE DBG messages of FSM SEND ADC DEBUG

#ifdef FSM_STATE_DBG
//static bool state_FSM_flag1;
//static bool state_FSM_flag2;
//static bool state_FSM_flag3;
//static bool state_FSM_flag4;
#endif

// Define the states for the state machine
typedef enum {
    STATE_REF_LAP,
    STATE_RUNNING,
    STATE_STOPPED,
    STATE_ERROR,
    // Add other states as needed
} State;

// VARIABLES

// FUNCTIONS
void car_control_simple(void);          // simple motor PWM ADC data controlling
void state_machine_init(void);          // Initialize the state machine
void car_control_FSM(void);             // a Finite State Machine for motor PWM controlling
void state_transition(State new_state); // Transition between states
void state_machine_reset(void);         // Reset State Machine
uint16_t feed_stored_data(const uint16_t *storedData, uint16_t dataLength); // goes thru saved data and feeds it as an output
void lap_counter(void);                 // when called it prints counted laps

#endif /* AUX_H_ */
