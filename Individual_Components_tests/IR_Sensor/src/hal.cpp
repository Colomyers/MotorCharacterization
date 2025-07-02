#include "hal.h"
#include <Arduino.h>
#include "driver/pcnt.h"
unsigned long old;

void init_HAL(){
    #define IR_SENSOR_PIN 27
    #define PCNT_UNIT PCNT_UNIT_0
    #define PCNT_H_LIM_VAL 10000
    #define PCNT_L_LIM_VAL -10000
    #define FILTER_VAL 1000 //necessary to calibrate 
    #define INTERVAL 16.67 //necessary to calibrate
    old = 0; 
};

void configPCNT() {
  // Configure pulse counter
  pcnt_config_t pcnt_config = {
    .pulse_gpio_num = IR_SENSOR_PIN,               /*!< Pulse input GPIO number, if you want to use GPIO16, enter pulse_gpio_num = 16, a negative value will be ignored */
    .ctrl_gpio_num = PCNT_PIN_NOT_USED, //not used /*!< Control signal input GPIO number, a negative value will be ignored */
    .lctrl_mode = PCNT_MODE_KEEP, //not used       /*!< PCNT low control mode */
    .hctrl_mode= PCNT_MODE_KEEP, //not used        /*!< PCNT high control mode */
    .pos_mode = PCNT_COUNT_INC,  //incremental     /*!< PCNT positive edge count mode */
    .neg_mode = PCNT_COUNT_DIS,  //Disabled        /*!< PCNT negative edge count mode */
    .counter_h_lim = PCNT_H_LIM_VAL, //maximum +   /*!< Maximum counter value */
    .counter_l_lim = PCNT_L_LIM_VAL, //maximum -   /*!< Minimum counter value */
    .unit =PCNT_UNIT,                              /*!< PCNT unit number */
    .channel = PCNT_CHANNEL_0,                     /*!< the PCNT channel */

  };

  pcnt_unit_config(&pcnt_config); // Initializes the PCNT unit

  //glitch correction(calibrate FILTER_VAL) -> short intervals
  pcnt_set_filter_value(PCNT_UNIT, FILTER_VAL);
  pcnt_filter_enable(PCNT_UNIT);


  //Starts the pulse counter

  pcnt_counter_pause(PCNT_UNIT);
  pcnt_counter_clear(PCNT_UNIT); 
  pcnt_counter_resume(PCNT_UNIT);

}


float calculate_RPM(int revolutions) {
  static int16_t count = 0;
  unsigned long now = micros(); 
  
  float delta = (now-old)/1e6;
  old = now; 
  pcnt_get_counter_value(PCNT_UNIT, &count);
  pcnt_counter_clear(PCNT_UNIT); 
  float calc_rpm = ((count/delta)/revolutions)*60.0; 
  return calc_rpm;

}
