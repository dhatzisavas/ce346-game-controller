#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "nrf_delay.h"
#include "nrfx_saadc.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"

#include "microbit_v2.h"

#define button_max_size 4

typedef struct button_arr { 
    int buttons[button_max_size]; /*not sure what type this will be, somehow contains button info. Maybe separate struct, button identifier + pin?*/
    uint8_t current_size;
} button_list_t;

button_list_t button_lst;

void list_insert(button_list_t* lst);

//configures GPIO pin, GPIOTE interrupt - active high
int init_button(uint32_t pin_number, void callback(nrfx_gpiote_pin_t, nrf_gpiote_polarity_t)); 

int read_button(uint32_t pin_number);