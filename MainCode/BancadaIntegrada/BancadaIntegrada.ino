#include "IR.h"
#include "HX711.h"
#include <ESP32Servo.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;

const int LowerPPMlimit = 1012;
const int UpperPPMlimit = 2020;

unsigned long TempoInicial = 0;
unsigned long TempoAnterior = 0;
const int IntervaloLeitura = 300; 

long PWM_percentage = 0;
unsigned long TempoPWM = 0;
bool FINISH = false;

HX711 scale;
Servo ESC;

void setup() {
  ESC.attach(12,1012,2020); // Configura o ESC no pino 12 com os limites de 1012 a 2020 us
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // inicial a célula de carga com seus respectivos pinos
  Serial.begin(115200); // inicia comunicação serial

  scale.set_scale(992.536); // Determina a escala da célula de carga, valor obtido através da calibração                      
  scale.tare();	// realiza a tara da célula de carga

  init_HAL();
  configPCNT();

  // Cabeçalho para aquisição de dados
  Serial.println("CLEARSHEET");
  Serial.println("LABEL,Tempo (s),PWM (%),Empuxo (g),Corrente (A),Tensao (V),Velocidade (RPM)");

  ESC.write(1012); // envia o sinal para iniciar o ESC
  delay(3000); // espera o ESC iniciar
  
  TempoInicial = millis();
  TempoAnterior = TempoInicial;
}





void loop() {
  unsigned long agora = millis();
  int x = 0;
  float rpm_avg; 
  while (x< 101) {
    float rpm = calculate_RPM(1);
    delay(5); //change this value to calibrate, works well with 5 and 50
    rpm_avg = moving_AVG(rpm);
    x++;
  }
  long PWM_input = map(PWM_percentage,0,100,LowerPPMlimit,UpperPPMlimit);
  ESC.write(PWM_input);

  if (PWM_percentage > 101){
    FINISH = true;
    PWM_percentage = 0;
  }
  if (agora - TempoAnterior >= IntervaloLeitura){
    Serial.print("DATA,");
    Serial.print((agora - TempoInicial) / 1000.0, 3); Serial.print(",");
    Serial.print(PWM_percentage);                     Serial.print(",");
    Serial.print(scale.get_units(10), 1);             Serial.print(",");
    Serial.print(0);                                  Serial.print(",");
    Serial.print(0);                                  Serial.print(",");
    Serial.println(rpm_avg);
    TempoAnterior = millis();
  }
  if ((agora - TempoPWM > 5000) && FINISH == false){
    PWM_percentage = PWM_percentage +1;
    TempoPWM = millis();
  }
  
  
}
