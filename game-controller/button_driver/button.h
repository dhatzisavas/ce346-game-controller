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

//configures GPIO pin, GPIOTE interrupt - active high
int init_button(uint32_t pin_number, void callback(nrfx_gpiote_pin_t, nrf_gpiote_polarity_t)); 

int read_button(uint32_t pin_number);