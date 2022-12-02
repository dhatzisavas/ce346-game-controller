#include "haptic.h"

NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);
_APP_TIMER_DEF(timer);

int main(){
  // Initialize I2C peripheral and driver
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = EDGE_P19;
  i2c_config.sda = EDGE_P20;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;
  printf("init twi manager\n");
  nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  printf("init app timer library\n");
  app_timer_init();
  i2c_init(twi_mngr_instance);
  printf("init done\n");
  if(haptic_init(&timer)){
    printf("Failed!\n");
    return 0;
  }
  else {
    printf("Success!\n");
  }
  haptic_config();
  printf("starting\n");
  if(haptic_start()){
    printf("starting failure!\n");
  }
  nrf_delay_ms(5000);
  printf("stopping\n");
  haptic_stop();
  nrf_delay_ms(1000);
  haptic_resume();
  nrf_delay_ms(2000);
  haptic_stop();
  nrf_delay_ms(1000);
  printf("timed\n");
  haptic_timed(1000);
}