#include "button.h"

int init_button(uint32_t pin_number, void callback(nrfx_gpiote_pin_t, nrf_gpiote_polarity_t)) {
    nrf_gpio_pin_dir_set(pin_number, 0); 

    //initialize gpiote
    nrfx_gpiote_init();

    //configure
    nrfx_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    uint32_t success = nrfx_gpiote_in_init(pin_number, &in_config, callback);
    // printf("Exit code: 0x%x. Should be NRFX_SUCCESS (0x0).\n", success);
    if (success != NRFX_SUCCESS) {
        // printf("could not set up interrupt\n");
        return -1;
    }

    nrfx_gpiote_in_event_enable(pin_number, true);  // enable interrupts

    return 0;
}

int read_button(uint32_t pin_number) {
    return nrf_gpio_pin_read(pin_number);
}