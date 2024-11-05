/*
 * flash_memory.h
 *
 *  Created on: 05/November/2024
 *      Author: xkosik09
 */

#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

// INCLUDES
#include <msp430.h>
#include <stdint.h>

// DEFINITIONS
#define TEST_DATA_SAMPLE_COUNT 6
#define FLASH_START_ADDR 0x18000 // Adjust based on available flash segment

// VARIABLES

// FUNCTIONS
void flash_write_data(uint16_t* data, uint32_t length);
void flash_read_data(uint16_t* buffer, uint32_t length);
void test_flash_read_write(void);


#endif // FLASH_MEMORY_H
