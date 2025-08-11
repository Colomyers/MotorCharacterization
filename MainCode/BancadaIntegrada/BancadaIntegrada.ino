#include "IR.h"
#include "HX711.h"
#include <ESP32Servo.h>
#include <Wire.h>
#include <INA226_WE.h>
#define I2C_ADDRESS 0x40

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;

const int LowerPPMlimit = 1012;
const int UpperPPMlimit = 2020;

unsigned long TempoInicial = 0;
unsigned long TempoAnterior = 0;
const int IntervaloLeitura = 2000;


unsigned long inicioaquisicao = 0;
long PWM_percentage = 0;
unsigned long TempoPWM = 0;
unsigned long TempoRPM = 0;
bool FINISH = false;

float shuntVoltage_mV = 0.0;
float loadVoltage_V = 0.0;
float busVoltage_V = 0.0;
float current_mA = 0.0;
float power_mW = 0.0;
long PWM_input = 1012;
int x = 0;
float rpm_avg; 
HX711 scale;
Servo ESC;
INA226_WE ina226 = INA226_WE(I2C_ADDRESS);

void setup() {
  Serial.begin(115200); // inicia comunicação serial
  Wire.begin(); // inicia comunicação i2c e aguarda conexão do INA226
  if(!ina226.init()){
    Serial.println("Failed to init INA226. Check your wiring.");
    while(1){}
  }

  ESC.attach(12,1012,2020); // Configura o ESC no pino 12 com os limites de 1012 a 2020 us

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // inicial a célula de carga com seus respectivos pinos
  scale.set_scale(992.536); // Determina a escala da célula de carga, valor obtido através da calibração                      
  scale.tare();	// realiza a tara da célula de carga

  ina226.setAverage(AVERAGE_4);
  ina226.setConversionTime(CONV_TIME_1100);
  ina226.setResistorRange(0.00375,20);
  ina226.setCorrectionFactor(0.985);

  init_HAL();
  configPCNT();

  // Cabeçalho para aquisição de dados
  Serial.println("CLEARSHEET");
  Serial.println("LABEL,Tempo (s),PWM ,Empuxo (g),Corrente (mA),Tensao (V),Potencia (mW),Velocidade (RPM)");

  ESC.write(1012); // envia o sinal para iniciar o ESC
  delay(8000); // espera o ESC iniciar
  
  TempoInicial = millis();
  TempoAnterior = TempoInicial;
}

void loop() {

  if (inicioaquisicao == 0){
    inicioaquisicao = millis();
  }

  if (FINISH == false){
    PWM_input = (1.6*(millis()-inicioaquisicao)/1000) + 1012;
    ESC.write(PWM_input);
  }
  

  if (PWM_input > 2020){

    FINISH = true;
    ESC.write(1012);
  }

  int x = 0;
  float rpm_avg; 

  while (x< 101) {
    float rpm = calculate_RPM(1);
    delay(5); //change this value to calibrate, works well with 5 and 50
    rpm_avg = moving_AVG(rpm);
    x++;
  }

  if (millis() - TempoAnterior >= IntervaloLeitura){
    
    
    
    shuntVoltage_mV = ina226.getShuntVoltage_mV();
    busVoltage_V = ina226.getBusVoltage_V();
    current_mA = ina226.getCurrent_mA();
    power_mW = ina226.getBusPower();
    loadVoltage_V  = busVoltage_V + (shuntVoltage_mV/1000);

    Serial.print("DATA,");
    Serial.print((millis() - TempoInicial) / 1000.0, 3); Serial.print(",");
    Serial.print(PWM_input);                        Serial.print(",");
    Serial.print(scale.get_units(10), 1);                Serial.print(",");
    Serial.print(current_mA);                            Serial.print(",");
    Serial.print(loadVoltage_V);                         Serial.print(",");
    Serial.print(power_mW);                              Serial.print(",");
    Serial.println(rpm_avg);
    TempoAnterior = millis();

   
    
  }
}
