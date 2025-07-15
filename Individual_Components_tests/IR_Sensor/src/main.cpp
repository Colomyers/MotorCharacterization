#include <Arduino.h>
#include "driver/pcnt.h"
#include "hal.h"
#include "ESP32Servo.h"

Servo ESC;

void setup() {
  ESC.attach(12,1012,2020);
  Serial.begin(115200); 
  init_HAL();
  configPCNT();
  ESC.write(1012); // envia o sinal para iniciar o ESC
  delay(3000); // espera o ESC iniciar
}

void loop() {
  ESC.write(1200);
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

