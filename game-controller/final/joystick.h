#pragma once

#include "i2c_lib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "app_timer.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

#include "nrfx_timer.h"
#include "microbit_v2.h"

#define JOYSTK 0x20
#define WHOAMI_VAL JOYSTK

enum POS_VALS {
    H_RIGHT = 0,
    V_BOTTOM = 0,
    H_MID = 496,
    V_MID = 544,
    H_LEFT = 1020,
    V_TOP = 1020,
    H_MID_LOW = 124,
    V_MID_LOW = 129,
    H_LEFT_LOW = 255,
    V_TOP_LOW = 255
};

enum DIRS {
    NONE = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

typedef struct joystick_vals {
    uint16_t horizontal;
    uint16_t vertical;
} joystick_vals_t;

enum REGS {
    WHOAMI = 0x00,
    FIRMWARE_1,
    FIRMWARE_2,
    HORIZONTAL_MSB,
    HORIZONTAL_LSB,
    VERTICAL_MSB,
    VERTICAL_LSB,
    BUTTON_POS,
    BUTTON_STATUS,
    LOCK_REG,
    I2C_ADDR
};

int joystick_init(const app_timer_id_t created_timer, bool high_accuracy_mode);

joystick_vals_t read_joystick();

uint16_t read_joystick_horizontal();

uint16_t read_joystick_vertical();

joystick_vals_t read_joystick_low();

uint8_t read_joystick_horizontal_low();

uint8_t read_joystick_vertical_low();