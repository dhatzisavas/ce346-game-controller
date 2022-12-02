#pragma once
#include "microbit_v2.h"

//applies a mask -- only modifying selectedbits
//bit start - most significant bit of data
//len - number of bits in data
int i2c_write_by_bit(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data_val, uint8_t bit_start, uint8_t bits_len);

uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr) ;

static void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);