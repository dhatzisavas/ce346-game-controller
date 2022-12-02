#include "button.h"
#include "haptic.h"
#include "joystick.h"

_APP_TIMER_DEF(haptic_timer);
_APP_TIMER_DEF(joystick_timer);

NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);


int num_buttons = 1;
int buttons[] = {EDGE_P13};

uint8_t dev_values[6] = {0};

#define OUTPUT_VALS(x); \ 
    putchar(dev_values[0]); \
    putchar(dev_values[1]); \
    putchar(dev_values[2]); \
    putchar(dev_values[3]); \
    putchar(dev_values[4]); \
    putchar(dev_values[5]);

int main() {
    //init once
    nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
    i2c_config.scl = EDGE_P19;
    i2c_config.sda = EDGE_P20;
    i2c_config.frequency = NRF_TWIM_FREQ_100K;
    i2c_config.interrupt_priority = 0;
    printf("init twi manager\n");
    nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
    i2c_init(twi_mngr_instance);
    app_timer_init();

    //device init
    for(int i = 0; i < num_buttons; i++){
        init_button(buttons[i], NULL); //todo: add callback
    }
    haptic_init(&haptic_timer);
    joystick_init(&haptic_timer, false);
    while(true){
        char in = getchar();
        if(in == 'a'){
            dev_values[0] = read_joystick_horizontal_low();
            dev_values[1] = read_joystick_vertical_low();
            dev_values[2] = read_button(buttons[0]);
        }
        OUTPUT_VALS(dev_vals);
    }
}