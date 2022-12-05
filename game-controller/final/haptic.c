#include "haptic.h"

uint8_t top_ctl1_saved;

bool first = true;

app_timer_id_t timer;

// returns 0 if whoami register matches expected value
//-1 otherwise
int haptic_init(const app_timer_id_t created_timer) {
    timer = created_timer;

    // checking whoami register
    uint8_t read = i2c_reg_read(HAPTIC_MOTOR_ADDR, CHIP_REV_REG);
    if (read != WHOAMI_HAPTIC) {
        return -1;
    }

    // set mode to i2c - DRO(direct register overide) mode
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL1, 1);
    
    return 0;
}

int haptic_config(h_config_t conf) {
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL2, 0x1F);
    return 0;
}

int haptic_start() {
    i2c_write_by_bit(HAPTIC_MOTOR_ADDR, TOP_CTL1, 1, 4, 1);
    first = false;
    return 0;
}

void haptic_stop() {
    // hack to make sure it can resume later
    top_ctl1_saved = i2c_reg_read(HAPTIC_MOTOR_ADDR, TOP_CTL1);
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL1, 0);
}

int haptic_resume() {
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL1, top_ctl1_saved);
    return 0;
}

// runs the haptic driver for a duration of ms, in milliseconds
int haptic_timed(uint32_t ms) {
    if (first) {
        haptic_start();
        first = false;
    } else {
        haptic_resume();
    }
    app_timer_create(&timer, APP_TIMER_MODE_SINGLE_SHOT, haptic_stop);
    app_timer_start(timer, APP_TIMER_TICKS(ms), NULL);
    return 0;
}
