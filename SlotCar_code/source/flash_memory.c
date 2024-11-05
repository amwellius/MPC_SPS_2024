/*
 * flash_memory.c
 *
 *  Created on: 05/November/2024
 *      Author: xkosik09
 */

// INCLUDES
#include "include/flash_memory.h"
#include "include/UART.h"

// VARIABLES
uint16_t test_input_data[TEST_DATA_SAMPLE_COUNT] = {11, 12, 13, 14, 15, 16};

// FUNCTIONS
void flash_write_data(uint16_t* data, uint32_t length) {
    uint16_t* flash_ptr = (uint16_t*)FLASH_START_ADDR; // Flash start pointer
    uint32_t i;

    // Erase the flash segment before writing
    FCTL3 = FWKEY;                   // Clear lock bit
    FCTL1 = FWKEY + ERASE;           // Set erase bit
    *flash_ptr = 0;                  // Dummy write to trigger erase
    FCTL1 = FWKEY + WRT;             // Enable write mode

    for (i = 0; i < length; i++) {
        *flash_ptr++ = data[i];      // Write each sample to flash
        __no_operation();            // Small delay for each write, if needed
    }

    FCTL1 = FWKEY;                   // Clear write bit
    FCTL3 = FWKEY + LOCK;            // Set lock bit to prevent accidental writes
}

void flash_read_data(uint16_t* buffer, uint32_t length){
    uint16_t* flash_ptr = (uint16_t*)FLASH_START_ADDR;
    uint32_t i;

    for (i = 0; i < length; i++) {
        buffer[i] = *flash_ptr++;    // Read each sample from flash
        ble_send_uint16(buffer[i]);
        ble_send("\n");
    }
    ble_send("*** flash data read ***");
    ble_send("\n\n");
}

void test_flash_read_write(void)
{
    // Write ADC data to flash memory
    flash_write_data(test_input_data, TEST_DATA_SAMPLE_COUNT);
    ble_send("*** temp data written to flash memory ***\n");

    // (Optional) Verify by reading back the data into a buffer
    uint16_t read_buffer[TEST_DATA_SAMPLE_COUNT];
    ble_send("*** reading flash memory ***\n");
    flash_read_data(read_buffer, TEST_DATA_SAMPLE_COUNT);

}
