#include "button.h"
#include "haptic.h"
#include "joystick.h"
#include "pthread.h"

_APP_TIMER_DEF(haptic_timer);
_APP_TIMER_DEF(joystick_timer);

NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

int num_buttons = 4;
int buttons[] = {EDGE_P3, EDGE_P9, EDGE_P15, EDGE_P16};

uint8_t dev_values[6] = {0};

bool drive = false;
bool ready = true;

#define OUTPUT_VALS(x)      \
    putchar(dev_values[0]); \
    putchar(dev_values[1]); \
    putchar(dev_values[2]); \
    putchar(dev_values[3]); \
    putchar(dev_values[4]); \
    putchar(dev_values[5])

static volatile void read_in(){
    char c = getchar();
    if(c && ready){
        drive = true;
        ready = false;
    }
    else if (ready && !drive){
        drive = false;
    }
}

void haptic_easy_time(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t polarity){
    haptic_timed(500);
}

int main() {
    // things to init once
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
    
    // init individual devices
    for (int i = 0; i < num_buttons; i++) {
        init_button(buttons[i], haptic_easy_time);  // todo: add callback
    }
    printf("buttons done\n");
    haptic_init(haptic_timer);
    printf("haptic done\n");
    joystick_init(joystick_timer, false);
    printf("devices inited\n");
    haptic_config();
    haptic_start();
    haptic_stop();
    haptic_timed(1000);
    nrf_delay_ms(1000);
    //loop for reading and sending values
    while (1) {
        // printf("looping\n");
        //code to try to take input for using haptic drive - hasn't worked
        char in = getchar();
        printf("0x%x\n", in);
        if(in == 'a'){
            printf("now\n");
            haptic_timed(500);
        }
        dev_values[0] = read_joystick_horizontal_low();
        dev_values[1] = read_joystick_vertical_low();
        dev_values[2] = read_button(buttons[0]);
        dev_values[3] = read_button(buttons[1]);
        dev_values[4] = read_button(buttons[2]);
        dev_values[5] = read_button(buttons[3]);
        OUTPUT_VALS(dev_vals);
        nrf_delay_ms(150);
        // printf("working\n");
    }
    printf("exiting\n");
}