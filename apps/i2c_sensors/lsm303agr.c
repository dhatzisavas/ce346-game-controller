// LSM303AGR driver for Microbit_v2
//
// Initializes sensor and communicates over I2C
// Capable of reading temperature, acceleration, and magnetic field strength

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "lsm303agr.h"
#include "nrf_delay.h"

// Pointer to an initialized I2C instance to use for transactions
static const nrf_twi_mngr_t* i2c_manager = NULL;

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
// Initialize and configure the LSM303AGR accelerometer/magnetometer
//
// i2c - pointer to already initialized and enabled twim instance
void lsm303agr_init(const nrf_twi_mngr_t* i2c) {
    i2c_manager = i2c;

    // ---Initialize Accelerometer---

    // Reboot acclerometer
    i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_CTRL_REG5, 0x80);
    nrf_delay_ms(100);  // needs delay to wait for reboot

    // Enable Block Data Update
    // Only updates sensor data when both halves of the data has been read
    i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_CTRL_REG4, 0x80);

    // Configure accelerometer at 100Hz, normal mode (10-bit)
    // Enable x, y and z axes
    i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_CTRL_REG1, 0x57);

    // Read WHO AM I register
    // Always returns the same value if working
    uint8_t result = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_WHO_AM_I_REG);
    // TODO: check the result of the Accelerometer WHO AM I register
    printf("Who am I register: 0x%x, should be 0x%x\n", result, 0x33);
    // ---Initialize Magnetometer---

    // Reboot magnetometer
    i2c_reg_write(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_CFG_REG_A, 0x40);
    nrf_delay_ms(100);  // needs delay to wait for reboot

    // Enable Block Data Update
    // Only updates sensor data when both halves of the data has been read
    i2c_reg_write(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_CFG_REG_C, 0x10);

    // Configure magnetometer at 100Hz, continuous mode
    i2c_reg_write(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_CFG_REG_A, 0x0C);

    // Read WHO AM I register
    result = i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_WHO_AM_I_REG);
    // TODO: check the result of the Magnetometer WHO AM I register
    printf("Who am I register: 0x%x, should be 0x%x\n", result, 0x40);

    // ---Initialize Temperature---

    // Enable temperature sensor
    i2c_reg_write(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_TEMP_CFG_REG, 0xC0);
    printf("Temperature: %f\n", lsm303agr_read_temperature());
}

// Read the internal temperature sensor
//
// Return measurement as floating point value in degrees C
float lsm303agr_read_temperature(void) {
    // TODO: implement me
    int16_t temp = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_TEMP_H);
    temp <<= 8;
    temp |= i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_TEMP_L);
    return (float)temp * (1.0 / 256.0) + 25.0;
}

float get_tilt();

void temp_handler() {
    printf("Temperature is %f\n", lsm303agr_read_temperature());
    lsm303agr_measurement_t accel = lsm303agr_read_accelerometer(), mag = lsm303agr_read_magnetometer();
    printf("Acceleration: {x: %f,y: %f,z: %f} g\n", accel.x_axis, accel.y_axis, accel.z_axis);
    printf("Magnetism : {x: %f,y: %f,z: %f} μΤ\n", mag.x_axis, mag.y_axis, mag.z_axis);
    printf("Tilt: %f degrees\n", get_tilt());

}

int16_t process_accel_vals(uint8_t low, uint8_t high) {
    low >>= 6;
    uint16_t val = (uint16_t)high;
    val <<= 2;
    val = val | low;
    int16_t signed_val = val << 6;
    signed_val >>= 6;
    return signed_val;
}

float read_accel_axis(axes_t axis){
    uint8_t address = LSM303AGR_ACC_ADDRESS;
    uint8_t reg_addr_l, reg_addr_h;
    switch (axis)
    {
    case XAXIS:
        reg_addr_l = LSM303AGR_ACC_OUT_X_L;
        reg_addr_h = LSM303AGR_ACC_OUT_X_H;
        break;
    case YAXIS:
        reg_addr_l = LSM303AGR_ACC_OUT_Y_L;
        reg_addr_h = LSM303AGR_ACC_OUT_Y_H;
        break;
    case ZAXIS:
        reg_addr_l = LSM303AGR_ACC_OUT_Z_L;
        reg_addr_h = LSM303AGR_ACC_OUT_Z_H;
        break;
    default:
        return 0;
        break;
    }
    int16_t rawval = process_accel_vals(i2c_reg_read(address, reg_addr_l), i2c_reg_read(address, reg_addr_h));
    return (float)rawval * 3.9 / 1000.0;
}

lsm303agr_measurement_t lsm303agr_read_accelerometer(void) {
    // TODO: implement me
    // lsm303agr_measurement_t measurement = {0};
    // uint8_t low = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Z_L);
    // uint8_t high = i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Z_H);
    // low >>= 6;
    // uint16_t val = (uint16_t)high;
    // val <<= 2;
    // val = val | low;
    // int16_t signed_val = val << 6;
    // signed_val >>= 6;
    // // printf("signed int value %x\n", signed_val);
    // float final_value = (float)signed_val * 3.9;
    // measurement.z_axis = final_value / 1000.0;

    // int16_t yval = process_accel_vals(i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Y_L), i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_Y_L));
    // final_value = (float) yval * 3.9;
    // measurement.y_axis = final_value / 1000.0

    // int16_t xval = process_accel_vals(i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_X_L), i2c_reg_read(LSM303AGR_ACC_ADDRESS, LSM303AGR_ACC_OUT_X_L));
    // final_value = float(xval) * 3.9;
    // measurement.x_axis = final_value / 1000.0
    lsm303agr_measurement_t measurement = {read_accel_axis(XAXIS), read_accel_axis(YAXIS), read_accel_axis(ZAXIS)};
    return measurement;
}

int16_t process_magnet_vals(uint8_t low, uint8_t high);

float read_magnet_axis(axes_t axis){
    uint8_t address = LSM303AGR_MAG_ADDRESS;
    uint8_t reg_addr_l, reg_addr_h;
    switch (axis)
    {
    case XAXIS:
        reg_addr_l = LSM303AGR_MAG_OUT_X_L_REG;
        reg_addr_h = LSM303AGR_MAG_OUT_X_H_REG;
        break;
    case YAXIS:
        reg_addr_l = LSM303AGR_MAG_OUT_Y_L_REG;
        reg_addr_h = LSM303AGR_MAG_OUT_Y_H_REG;
        break;
    case ZAXIS:
        reg_addr_l = LSM303AGR_MAG_OUT_Z_L_REG;
        reg_addr_h = LSM303AGR_MAG_OUT_Z_H_REG;
        break;
    default:
        return 0;
        break;
    }
    int16_t rawval = process_magnet_vals(i2c_reg_read(address, reg_addr_l), i2c_reg_read(address, reg_addr_h));
    return (float) rawval / 10.0;
}

int16_t process_magnet_vals(uint8_t low, uint8_t high){
    int16_t val = (int16_t) high << 2;
    val |= low;
    val <<= 6;
    val >>= 6;
    return val;
}

lsm303agr_measurement_t lsm303agr_read_magnetometer(void) {
    // TODO: implement me
    // lsm303agr_measurement_t measurement = {0};
    // uint8_t low = i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_Z_L_REG);
    // uint8_t high = i2c_reg_read(LSM303AGR_MAG_ADDRESS, LSM303AGR_MAG_OUT_Z_H_REG);
    // int16_t val = (int16_t)high << 2;
    // val |= low;
    // val <<= 6;
    // val >>= 6;
    // measurement.z_axis = (float) val / 10.0;
    lsm303agr_measurement_t measurement = {read_magnet_axis(XAXIS), read_magnet_axis(YAXIS), read_magnet_axis(ZAXIS)};
    return measurement;
}

float get_tilt(){
    lsm303agr_measurement_t accel = lsm303agr_read_accelerometer();
    float phi = atan(sqrt((accel.x_axis * accel.x_axis) + (accel.y_axis * accel.y_axis))/accel.z_axis);
    return phi * 180.00 / M_PI; 
}
