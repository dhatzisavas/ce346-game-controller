#include "joystick.h"

joystick_vals_t last_val = {0, 0};

app_timer_id_t timer;

uint32_t time = 100;

void update_val();

bool high_accuracy = true;

int joystick_init(const app_timer_id_t *created_timer, bool high_accuracy_mode) {
    if (created_timer) {
        timer = *created_timer;
    } else {
        return -1;
    }
    uint8_t read = i2c_reg_read(JOYSTK, WHOAMI);
    // printf("reg val: 0x%x\n", read);
    if (read != WHOAMI_VAL) {
        return -1;
    }
    app_timer_create(&timer, APP_TIMER_MODE_REPEATED, update_val);
    app_timer_start(timer, APP_TIMER_TICKS(time), NULL);
    high_accuracy = high_accuracy_mode;
    return 0;
}

joystick_vals_t read_joystick() {
    joystick_vals_t result = {read_joystick_horizontal(), read_joystick_vertical()};
    return result;
}

joystick_vals_t read_last_val() {
    return last_val;
}

joystick_vals_t read_joystick_low();

void update_val() {
    if (high_accuracy) {
        last_val = read_joystick();
    }
    else {
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

uint8_t read_joystick_horizontal_low(){
    return i2c_reg_read(JOYSTK, HORIZONTAL_MSB);
}

uint8_t read_joystick_vertical_low(){
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