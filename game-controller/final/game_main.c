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

char in_char = 0;

// only 6 values; might as well unroll the loop
// the weakness of C macros makes me miss lisp lol
#define OUTPUT_VALS(x)      \
    putchar(dev_values[0]); \
    putchar(dev_values[1]); \
    putchar(dev_values[2]); \
    putchar(dev_values[3]); \
    putchar(dev_values[4]); \
    putchar(dev_values[5])

// broken
// goal: if there is any input, and the state is ready, set flag to
// start haptic driver
static volatile void read_in() {
    char c = getchar();
    if (c && ready) {
        drive = true;
        ready = false;
    } else if (ready && !drive) {
        drive = false;
    }
}

// gpiote handler to call haptic_timed()
void haptic_easy_time(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t polarity) {
    haptic_timed(500);
}

static void alarm_handler(){
    in_char = 0;
}

int main()
{
    // things to init once
    nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
    i2c_config.scl = EDGE_P19;
    i2c_config.sda = EDGE_P20;
    i2c_config.frequency = NRF_TWIM_FREQ_100K;
    i2c_config.interrupt_priority = 0;
    if (nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config) != NRFX_SUCCESS) {
        printf("twi manager couldn't init\n");
        return -1;
    }

    i2c_init(twi_mngr_instance);

    if (app_timer_init() != NRFX_SUCCESS)
    {
        printf("app timer didn't init right\n");
        return -1;
    }

    // init individual devices
    for (int i = 0; i < num_buttons; i++)
    {
        init_button(buttons[i], haptic_easy_time);
    }
    
    joystick_init(joystick_timer, false);
    
    haptic_init(haptic_timer);
    haptic_config();
    //make sure driver is in a known state
    haptic_start();
    haptic_stop();

    //self test - if this doesn't happen, setup failed
    haptic_timed(1000);
    nrf_delay_ms(1000);

    // loop for reading and sending values

    while (1) {
        printf("looping\n");
        // code to try to take input for using haptic drive - hasn't worked
        // char in = getchar();
        // if (in == 'a') {
        //     haptic_timed(500);
        // }

        //read values of parts
        dev_values[0] = read_joystick_horizontal_low();
        dev_values[1] = read_joystick_vertical_low();
        dev_values[2] = read_button(buttons[0]);
        dev_values[3] = read_button(buttons[1]);
        dev_values[4] = read_button(buttons[2]);
        dev_values[5] = read_button(buttons[3]);

        OUTPUT_VALS(dev_vals);
        
        nrf_delay_ms(50);
    }
    //should never really hit this point
    printf("exiting\n");
}