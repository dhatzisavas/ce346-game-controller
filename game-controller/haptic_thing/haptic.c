#include "haptic.h"

static const nrf_twi_mngr_t* i2c_manager;

uint8_t top_ctl1_saved;

// Helper function to perform a 1-byte I2C read of a given register
//
// i2c_addr - address of the device to read from
// reg_addr - address of the register within the device to read
//
// returns 8-bit read value
static uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr) {
    uint8_t rx_buf = 0;
    nrf_twi_mngr_transfer_t const read_transfer[] = {
        NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, sizeof(reg_addr), NRF_TWI_MNGR_NO_STOP),
        NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, sizeof(rx_buf), NULL)};
    nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);

    return rx_buf;
}

// Helper function to perform a 1-byte I2C write of a given register
//
// i2c_addr - address of the device to write to
// reg_addr - address of the register within the device to write
static void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {
    // TODO: implement me
    // Note: there should only be a single two-byte transfer to be performed
    uint8_t arr[] = {reg_addr, data};
    nrf_twi_mngr_transfer_t const write_transfer[] = {
        NRF_TWI_MNGR_WRITE(i2c_addr, &arr, 2, 0)};
    nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
}



//returns 0 if whoami register matches expected value
//-1 otherwise
//what do we need to do for init? Turn on device? Config will be separate step
int haptic_init(const nrf_twi_mngr_t* i2c){
    i2c_manager = i2c;
    //checking whoami register
    uint8_t read = i2c_reg_read(HAPTIC_MOTOR_ADDR, CHIP_REV_REG);
    printf("WHOAMI: 0x%x\n", read);
    if(read != WHOAMI){
        return -1;
    }
    //set mode to i2c - DRO(direct register overide) mode
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL1, 1);
    return 0;
}

int haptic_config(h_config_t conf){
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL2, 0x7F);
    return 0;
}

int haptic_start() {
    i2c_write_by_bit(HAPTIC_MOTOR_ADDR, TOP_CTL1, 1, 4, 1);
    return 0;
}

int haptic_stop(){
    // i2c_write_by_bit(HAPTIC_MOTOR_ADDR, TOP_CTL1, 0, 4, 1);
    top_ctl1_saved = i2c_reg_read(HAPTIC_MOTOR_ADDR, TOP_CTL1);
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL1, 0);
    return 0;
}

int haptic_resume(){
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL1, top_ctl1_saved);
    return 0;
}

static int i2c_write_by_bit(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data_val, uint8_t bit_start, uint8_t bits_len){
    const uint8_t cur_val = i2c_reg_read(i2c_addr, reg_addr);
    uint8_t new_val = cur_val;
    uint8_t mask = 0xFF;
    mask = mask >> (8 - bits_len);
    printf("mask 1: 0x%x\n", mask);
    data_val &= mask; //want to make sure data is only the size it promises, otherwise will mess up bit operations
    mask <<= bit_start; //align mask
    mask = ~mask;
    new_val &= mask;
    new_val |= (data_val << bit_start);
    printf("Original value: 0x%x\nNew value: 0x%x\n", cur_val, new_val);
    i2c_reg_write(i2c_addr, reg_addr, new_val);
    if(new_val == i2c_reg_read(i2c_addr, reg_addr)){
        return 0;
    }
    else{
        return -1;
    }
}