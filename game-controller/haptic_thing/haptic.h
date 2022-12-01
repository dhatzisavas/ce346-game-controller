#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "nrf_delay.h"
#include "nrfx_saadc.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

#include "microbit_v2.h"
#include "nrf_twi_mngr.h"

/*
  Driver features inspired by https://github.com/sparkfun/SparkFun_Qwiic_Haptic_Driver_DA7280_Arduino_Library,
  REGISTERS copied directly (in public domain)
  written by Elias Santistevan.
*/

#define WHOAMI 0xBA
#define HAPTIC_MOTOR_ADDR 0x4A

enum REGISTERS {

    CHIP_REV_REG = 0x00,  // whoami?
    IRQ_EVENT1 = 0x03,
    IRQ_EVENT_WARN_DIAG,
    IRQ_EVENT_SEQ_DIAG,
    IRQ_STATUS1,
    IRQ_MASK1,
    CIF_I2C1,
    FRQ_LRA_PER_H = 0x0A,
    FRQ_LRA_PER_L,
    ACTUATOR1,
    ACTUATOR2,
    ACTUATOR3,
    CALIB_V2I_H,
    CALIB_V2I_L = 0x10,
    CALIB_IMP_H,
    CALIB_IMP_L,
    TOP_CFG1,
    TOP_CFG2,
    TOP_CFG3,
    TOP_CFG4,
    TOP_INT_CFG1,
    TOP_INT_CFG6_H = 0x1C,
    TOP_INT_CFG6_L,
    TOP_INT_CFG7_H,
    TOP_INT_CFG7_L,
    TOP_INT_CFG8 = 0x20,
    TOP_CTL1 = 0x22,
    TOP_CTL2,
    SEQ_CTL1,
    SWG_C1,
    SWG_C2,
    SWG_C3,
    SEQ_CTL2,
    GPI_0_CTL,
    GPI_1_CTL,
    GPI_2_CTL,
    MEM_CTL1,
    MEM_CTL2,
    ADC_DATA_H1,
    ADC_DATA_L1,
    POLARITY = 0x43,
    LRA_AVR_H,
    LRA_AVR_L,
    FRQ_LRA_PER_ACT_H,
    FRQ_LRA_PER_ACT_L,
    FRQ_PHASE_H,
    FRQ_PHASE_L,
    FRQ_CTL = 0x4C,
    TRIM3 = 0x5F,
    TRIM4,
    TRIM6 = 0x62,
    TOP_CFG5 = 0x6E,
    IRQ_EVENT_ACTUATOR_FAULT = 0x81,
    IRQ_STATUS2,
    IRQ_MASK2,
    SNP_MEM_X

};

typedef struct h_config {
    float nomVolt;
    float absVolt;
    float currMax;
    float impedance;
    float lraFreq;
} h_config_t;

//applies a mask -- only modifying selectedbits
//bit start - most significant bit of data
//len - number of bits in data
static int i2c_write_by_bit(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data_val, uint8_t bit_start, uint8_t bits_len);

// returns 0 if succesfully reads WHOAMI register
// returns -1 otherwise
int haptic_init(const nrf_twi_mngr_t* i2c);

// configures haptic driver
int haptic_config();

// Starts the haptic motor, based on setting in haptic_config - first time!
int haptic_start();

// Stops the haptic motor
int haptic_stop();

// Used to resume vibrating after haptic_stop;
int haptic_resume();

// Starts the haptic driver, running for time ms in milliseconds
// If already running, will continue until the timer elapses
int haptic_timed(uint32_t ms);
