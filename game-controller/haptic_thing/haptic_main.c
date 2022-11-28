#include "haptic.h"

NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

int main(){
  // Initialize I2C peripheral and driver
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = EDGE_P19;
  i2c_config.sda = EDGE_P20;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;
  nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  if(haptic_init(&twi_mngr_instance)){
    printf("Failed!\n");
  }
  else {
    printf("Success!\n");
  }
}