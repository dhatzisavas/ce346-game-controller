#include "joystick.h"

joystick_vals_t last_val = {0, 0};

app_timer_id_t timer;

uint32_t time = 100;

bool high_accuracy = true;

void update_val();

joystick_vals_t read_joystick_low();

// high accuracy: 10 bits of precision, low is 8
int joystick_init(const app_timer_id_t created_timer, bool high_accuracy_mode) {
    timer = created_timer;
    high_accuracy = high_accuracy_mode;
    
    uint8_t read = i2c_reg_read(JOYSTK, WHOAMI);
    if (read != WHOAMI_VAL) {
        return -1;
    }

    app_timer_create(&timer, APP_TIMER_MODE_REPEATED, update_val);
    app_timer_start(timer, APP_TIMER_TICKS(time), NULL);

    return 0;
}

joystick_vals_t read_joystick() {
    joystick_vals_t result = {read_joystick_horizontal(), read_joystick_vertical()};
    return result;
}

joystick_vals_t read_last_val() {
    return last_val;
}

void update_val() {
    if (high_accuracy) {
        last_val = read_joystick();
    } else {
        last_val = read_joystick_low();
    }
}

static uint16_t combine_vals(uint16_t msb, uint16_t lsb) {
    msb <<= 2;
    lsb >>= 2;
    return msb | lsb;
}

uint16_t read_joystick_horizontal() {
    uint16_t top_bits = i2c_reg_read(JOYSTK, HORIZONTAL_MSB);
    uint16_t bottom_bits = i2c_reg_read(JOYSTK, HORIZONTAL_LSB);
    return combine_vals(top_bits, bottom_bits);
}

uint8_t read_joystick_horizontal_low() {
    return i2c_reg_read(JOYSTK, HORIZONTAL_MSB);
}

uint8_t read_joystick_vertical_low() {
    return i2c_reg_read(JOYSTK, VERTICAL_MSB);
}

joystick_vals_t read_joystick_low() {
    joystick_vals_t result = {read_joystick_horizontal_low(), read_joystick_vertical_low()};
    return result;
}

uint16_t read_joystick_vertical() {
    uint16_t top_bits = i2c_reg_read(JOYSTK, VERTICAL_MSB);
    uint16_t bottom_bits = i2c_reg_read(JOYSTK, VERTICAL_LSB);
    return combine_vals(top_bits, bottom_bits);
}