#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "nrf_delay.h"
#include "nrfx_saadc.h"
#include "button.h"

#include "microbit_v2.h"

void red_callback(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t idk){
    printf("Yellow button pressed!\n");
    nrf_gpio_pin_clear(pin);
}

int main(){
    printf("start\n");
    if(init_button(EDGE_P13, red_callback)){
        return -1;
    }
    while(1){
        // printf("Button state: %i\n", read_button(EDGE_P13));
        nrf_delay_ms(1000);
    }
}