#include "i2c_lib.h"

nrf_twi_mngr_t i2c_manager;

void i2c_init(const nrf_twi_mngr_t i2c_mgr){
    i2c_manager = i2c_mgr;
}

// Helper function to perform a 1-byte I2C read of a given register
//
// i2c_addr - address of the device to read from
// reg_addr - address of the register within the device to read
//
// returns 8-bit read value
uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr) {
    uint8_t rx_buf = 0;
    nrf_twi_mngr_transfer_t const read_transfer[] = {
        NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, sizeof(reg_addr), NRF_TWI_MNGR_NO_STOP),
        NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, sizeof(rx_buf), NULL)};
    nrf_twi_mngr_perform(&i2c_manager, NULL, read_transfer, 2, NULL);
    return rx_buf;
}

// Helper function to perform a 1-byte I2C write of a given register
//
// i2c_addr - address of the device to write to
// reg_addr - address of the register within the device to write
void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {
    uint8_t arr[] = {reg_addr, data};
    nrf_twi_mngr_transfer_t const write_transfer[] = {
        NRF_TWI_MNGR_WRITE(i2c_addr, &arr, 2, 0)};
    nrf_twi_mngr_perform(&i2c_manager, NULL, write_transfer, 1, NULL);
}

// reads current value of register, modified only selected bits, writes back
int i2c_write_by_bit(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data_val, uint8_t bit_start, uint8_t bits_len){
    const uint8_t cur_val = i2c_reg_read(i2c_addr, reg_addr);
    uint8_t new_val = cur_val;
    uint8_t mask = 0xFF;
    mask = mask >> (8 - bits_len);
    data_val &= mask; //want to make sure data is only the size it promises, otherwise will mess up bit operations
    mask <<= bit_start; //align mask
    mask = ~mask;
    new_val &= mask;
    new_val |= (data_val << bit_start);
    i2c_reg_write(i2c_addr, reg_addr, new_val);
    //verify write occured
    if(new_val == i2c_reg_read(i2c_addr, reg_addr)){
        return 0;
    }
    else{
        return -1;
    }
}