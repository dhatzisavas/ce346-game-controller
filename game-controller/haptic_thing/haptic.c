#include "haptic.h"

uint8_t top_ctl1_saved;

bool first = true;

app_timer_id_t timer;

//returns 0 if whoami register matches expected value
//-1 otherwise
//what do we need to do for init? Turn on device? Config will be separate step
int haptic_init(const app_timer_id_t *created_timer){
    timer = *created_timer;
    //checking whoami register
    uint8_t read = i2c_reg_read(HAPTIC_MOTOR_ADDR, CHIP_REV_REG);
    printf("WHOAMI: 0x%x\n", read);
    if(read != WHOAMI){
        return -1;
    }
    //set mode to i2c - DRO(direct register overide) mode
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL1, 1);
    printf("haptic init done, returning now\n");
    return 0;
}

int haptic_config(h_config_t conf){
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL2, 0x1F);
    return 0;
}

int haptic_start() {
    i2c_write_by_bit(HAPTIC_MOTOR_ADDR, TOP_CTL1, 1, 4, 1);
    first = false;
    return 0;
}

void haptic_stop(){
    // i2c_write_by_bit(HAPTIC_MOTOR_ADDR, TOP_CTL1, 0, 4, 1);
    top_ctl1_saved = i2c_reg_read(HAPTIC_MOTOR_ADDR, TOP_CTL1);
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL1, 0);
}

int haptic_resume(){
    i2c_reg_write(HAPTIC_MOTOR_ADDR, TOP_CTL1, top_ctl1_saved);
    return 0;
}

//use app timer
int haptic_timed(uint32_t ms){
    //setup timer here, haptic_stop should be the interrupt handler - need anything else?
    app_timer_create(&timer, APP_TIMER_MODE_SINGLE_SHOT, haptic_stop);
    app_timer_start(timer, APP_TIMER_TICKS(ms), NULL);
    if(first){
        haptic_start();
    }
    else {
        haptic_resume();
    }
    return 0;
}

