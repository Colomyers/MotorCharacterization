#include <Arduino.h>
#include "driver/pcnt.h"
#include "hal.h"

void setup() {
  
  Serial.begin(115200); 
  init_HAL();
  configPCNT();

}

void loop() {

  float rpm = calculate_RPM(2);

  Serial.print("rpm: ");
  Serial.println(rpm);
  delay(16.67);
  
}

