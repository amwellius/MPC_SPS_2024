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
         * looks like middle value it 1964. There also might be a bit of offset.
         * PROBLEM: When the motor is running, the ADC data fluctuates - is in noise,
         * so the speed adjusts accordingly. However, this is only noise not a real data reading.
         */
        switch(z_axis)
        {
        case 0 ... 1951:    // left
            LED_RL_ON();
            LED_RR_OFF();
            motor_pwm(PWM_LEVEL_2);
            break;
        case 1968 ... 4096: // right
            LED_RL_OFF();
            LED_RR_ON();
            motor_pwm(PWM_LEVEL_2);
            break;
        default:
            LED_RL_OFF();
            LED_RR_OFF();
            motor_pwm(PWM_LEVEL_4);
            break;
        }
    #endif

        // Send data over UART BLUETOOTH
        ble_send_uint16(z_axis);
        ble_send("\n");

        flag_62ms = 0;
    }
}








// **************************************INTERUPTS************************************** //
