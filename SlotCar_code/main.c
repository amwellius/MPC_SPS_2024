#include <msp430.h>
#include "include/timers.h"
#include "include/SPI.h"
#include "include/I2C.h"
#include "include/LED.h"
#include "include/ADC.h"
#include "include/L3GD20H.h"
#include "include/ADXL343.h"
#include "include/motor.h"
#include "include/UART.h"

//#define SPI
//#define I2C
//#define I2C_RX_BUFFER_SIZE 5
/**
 * main.c
 */

uint16_t i, index;
// I2C
//uint8_t RX_buffer[I2C_RX_BUFFER_SIZE] = {0};
//uint8_t RXByteCtr = 0;
//uint8_t ReceiveIndex = 0;
/*
uint8_t TransmitBuffer[MAX_BUFFER_SIZE] = {0};
uint8_t TXByteCtr = 0;
uint8_t TransmitIndex = 0;
*/



int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    initClockTo16MHz();         // initialize clock timer 16 MHz
    __bis_SR_register(GIE);     // Enable global interrupts
    _BIS_SR(GIE);        // some interrupts as well

#ifdef SPI
    uint8_t SPIData;
    SPI_init();
#endif

    // *** The user code starts here ***
    LED_init();
    init_timerB0();
    motor_init();
//    motor_pwm(PWM_LEVEL_3);

    ADC_init();
    ADC_start();
    UART_init();

    uint16_t x_axis = 0;
    uint16_t y_axis = 0;
    uint16_t z_axis = 0;

    // infinite loop
    while (1) {
        if (flag_62ms) {
            LED_FR_toggle();
            // ADC operation
            // Get the results using the getter function
            x_axis = ADC_get_result(2);
            y_axis = ADC_get_result(3);
            z_axis = ADC_get_result(4);

            // left / right
            switch(z_axis)
            {
            case 0 ... 1952:    // left
                LED_RL_ON();
                LED_RR_OFF();
                motor_pwm(PWM_LEVEL_4);
                break;
            case 1962 ... 4096: // right
                LED_RL_OFF();
                LED_RR_ON();
                motor_pwm(PWM_LEVEL_4);
                break;
            default:
                LED_RL_OFF();
                LED_RR_OFF();
                motor_pwm(PWM_LEVEL_6);
                break;
            }
            flag_62ms = 0;
        }

        if (flag_500ms) {               // Check if flag is set
//            LED_FR_toggle();
//            motor_pwm(PWM_LEVEL_1);
            flag_500ms = 0;             // Clear the flag
        }

        if (flag_1000ms) {
            LED_FL_toggle();
//            motor_pwm(PWM_LEVEL_2);

            // Sending data BLUETOOTH
            if (povol_TX == 1)
            {
                UCA1TXBUF = z_axis;   // data jsou ihned po vlozeni do UCAxTXBUF odeslana na seriovou branu
//                UCA1TXBUF = 77; // test letter M ASCII

                /*
                 * clean up the code - create aux.c file?
                 * ADC better range
                 * look into command line
                 *
                 */
            }

            flag_1000ms = 0;
        }




        // forward / backward
//        switch(y_axis)
//        {
//        case 0 ... 2125:    // forward
//            LED_RL_ON();
//            LED_RR_OFF();
//            break;
//        case 2135 ... 4096: // backwards
//            LED_RL_OFF();
//            LED_RR_ON();
//            break;
//        default:
//            LED_RL_ON();
//            LED_RR_ON();
//            break;
//        }

        // up / down
        // doenst work properly. We dont need this
//        switch(x_axis)
//        {
//        case 0 ... 2300:    // stable
//            LED_RL_ON();
//            LED_RR_OFF();
//            break;
//        case 2310 ... 4096: //
//            LED_RL_OFF();
//            LED_RR_ON();
//            break;
//        default:
//            LED_RL_ON();
//            LED_RR_ON();
//            break;
//        }
    }



//#ifdef I2C
//    I2C_init(0x20>>1);
//    _BIS_SR(GIE);
//    P3DIR &= ~0x04;                            // P3.2 as input
// /* only for ADXL343
//    P3DIR |= 0x09;                            // P3.2 as output for I2C address specification
//    P3OUT &= ~0x08;                           // P3.2 to LOW (for 1101010b of L3GD20H address selection)
//    P3OUT |= 0x01;                            // ADXL343 CS pin to HIGH for I2C enable
//    */
//#endif


//////////////////////////////////////////////////////////////////////////////////

//    while(1)
//    {
// //       P8OUT = 0b001100;
// //       for (i = 5000; i!=0; i--);
// //       P8OUT = 0b0;    // OFF
// //       for (i = 5000; i!=0; i--);
////        P8OUT = 0b0001100;
// /*       for (i = 1000; i!=0; i--);
//        P8OUT = 0b0;    // OFF
//        for (i = 1000; i!=0; i--);
//*/
//        _delay_cycles(5000);
//#ifdef SPI
//        SPIData = SPI_read_byte(WHO_AM_I | L3GD20H_READ);
//        SPIData = SPI_read_byte(CTRL1 | L3GD20H_READ);
//        SPI_write_byte(CTRL1 | L3GD20H_WRITE, 0x0F);
//        SPIData = SPI_read_byte(CTRL1 | L3GD20H_READ);
//#endif
//#ifdef I2C
//        __delay_cycles(1000);                     // Delay required between transaction
//        I2C_write_byte(0x00, 0x00);             // wake up signal at address 0x00
//        I2C_read_byte(0x00, 5);
//        // for ADXL343
//        /*
//        index = I2C_read_byte(BW_RATE);
//        I2C_write_byte(BW_RATE, index^0x01);
//        index = I2C_read_byte(BW_RATE);
//        */
//#endif
//     }
} // **** END OF MAIN FUNCTION ****




/// interupts
//#pragma vector = USCI_B0_VECTOR
//__interrupt void USCI_B0_ISR(void)
//{
//  uint8_t rx_val = 0;
//
//  switch(__even_in_range(UCB0IV,12))
//  {
//  case  0: break;                           // Vector  0: No interrupts
//  case  2: break;                           // Vector  2: ALIFG
//  case  4: break;                           // Vector  4: NACKIFG
//  case  6: break;                           // Vector  6: STTIFG
//  case  8: break;                           // Vector  8: STPIFG
//  case USCI_I2C_UCRXIFG:
//        rx_val = UCB0RXBUF;
//        if (RXByteCtr)
//        {
//          RX_buffer[ReceiveIndex++] = rx_val;
//          RXByteCtr--;
//        }
//
//        if (RXByteCtr == 1)
//        {
//          UCB0CTL1 |= UCTXSTP;
//        }
//        else if (RXByteCtr == 0)
//        {
//          UCB0IE &= ~UCRXIE;
//        }
//        break;                      // Interrupt Vector: I2C Mode: UCRXIFG
//  case 12: break;                           // Vector 12: TXIFG
//  default: break;
//  }
//}

//**********************************************************************************************




