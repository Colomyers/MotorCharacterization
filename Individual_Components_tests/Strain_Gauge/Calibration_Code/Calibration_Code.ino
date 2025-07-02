/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-load-cell-hx711/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Calibrating the load cell
#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;

HX711 scale;

void setup() {
  Serial.begin(115200);
  // rtc_cpu_freq_config_t config; // Cria a var config do tipo "rtc_cpu_freq_config_t" responsável por conter os parâmetros para configurar
  //                               // que configuram a frequência da CPU, como fonte do Clock e velocidade da CPU
  // rtc_clk_cpu_freq_get_config(&config); // Esta função lê as configurações de frequência atual da CPU e as guarda na variável config
  // rtc_clk_cpu_freq_to_config(RTC_CPU_FREQ_160M, &config); // Altera as configurações na variável config para operar em 80MHz
  // rtc_clk_cpu_freq_set_config_fast(&config); // Pega as configurações atualizadas na var config e aplica nos registros internos 
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void loop() {

  if (scale.is_ready()) {
    scale.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);
  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}

//calibration factor will be the (reading)/(known weight)
