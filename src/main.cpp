#include <Arduino.h>
#include "driver/pcnt.h"
#include "hal.h"

void setup() {
  
  Serial.begin(115200); 
  init_HAL();
  configPCNT();

}

void loop() {
  int x = 0;
  float rpm_avg; 
 while (x< 101) {
  float rpm = calculate_RPM(2);
  delay(5); //change this value to calibrate, works well with 5 and 50
  rpm_avg = moving_AVG(rpm);
  x++;
 }
  Serial.print("rpm: ");
  Serial.println(rpm_avg);
  //delay(50);
}

