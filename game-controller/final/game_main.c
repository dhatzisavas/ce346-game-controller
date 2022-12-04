#include "button.h"
#include "haptic.h"
#include "joystick.h"

_APP_TIMER_DEF(haptic_timer);
_APP_TIMER_DEF(joystick_timer);

NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

int num_buttons = 4;
int buttons[] = {EDGE_P3, EDGE_P9, EDGE_P15, EDGE_P16};

uint8_t dev_values[6] = {0};

#define OUTPUT_VALS(x)      \
    \ 
    putchar(dev_values[0]); \
    putchar(dev_values[1]); \
    putchar(dev_values[2]); \
    putchar(dev_values[3]); \
    putchar(dev_values[4]); \
    putchar(dev_values[5])

int main() {
    // init once
    nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
    i2c_config.scl = EDGE_P19;
    i2c_config.sda = EDGE_P20;
    i2c_config.frequency = NRF_TWIM_FREQ_100K;
    i2c_config.interrupt_priority = 0;
    printf("init twi manager\n");
    if (nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config) != NRFX_SUCCESS) {
        printf("twi manager couldn't init\n");
    }
    i2c_init(twi_mngr_instance);
    if (app_timer_init() != NRFX_SUCCESS) {
        printf("app timer didn't init right\n");
    }
    printf("all overall init done\n");
    // device init
    for (int i = 0; i < num_buttons; i++) {
        init_button(buttons[i], NULL);  // todo: add callback
    }
    printf("buttons done\n");
    haptic_init(&haptic_timer);
    printf("haptic done\n");
    joystick_init(&joystick_timer, false);
    printf("devices inited\n");
    haptic_config();
    haptic_timed(10000);
    while (1) {
        // printf("looping\n");
        // char in = getchar();
        // if(in == 'a'){
        dev_values[0] = read_joystick_horizontal_low();
        dev_values[1] = read_joystick_vertical_low();
        dev_values[2] = read_button(buttons[0]);
        dev_values[3] = read_button(buttons[1]);
        dev_values[4] = read_button(buttons[2]);
        dev_values[5] = read_button(buttons[3]);
        // }
        OUTPUT_VALS(dev_vals);
        nrf_delay_ms(100);
        // printf("working\n");
    }
    printf("exiting\n");
}