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
void car_control_simple(void);      // simple motor PWM ADC data controlling
void state_machine_init(void);      // Initialize the state machine
void car_control_FSM(void);         // a Finite State Machine for motor PWM controlling
void state_transition(State new_state); // Transition between states

#endif /* AUX_H_ */
