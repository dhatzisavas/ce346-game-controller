#include "joystick.h"

NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);
_APP_TIMER_DEF(timer);

int main() {
    nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
    i2c_config.scl = EDGE_P19;
    i2c_config.sda = EDGE_P20;
    i2c_config.frequency = NRF_TWIM_FREQ_100K;
    i2c_config.interrupt_priority = 0;
    nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
    i2c_init(twi_mngr_instance);
    if(joystick_init(&timer, false)){
        // printf("whoami doesn't match\n");
    }
   read_joystick_horizontal();
   while(1){
    joystick_vals_t x = read_joystick_low();
    putchar(x.horizontal);
    putchar(x.vertical);
    // putchar(0);
    // printf("horizontal: %i, vertical %i\n", x.horizontal, x.vertical);
   } 
}