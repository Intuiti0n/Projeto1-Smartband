/* PROJETO 1 SMART BAND, EEIC, 4ANO,
  SENSOR DE PULSAÇAO, ENVIAR POR PORTA SERIE
    AMOSTRAGEM 10 MS COM FILTRO (Fa=100Hz)
    FILTRO BUTTERWORTH 2A ORDEM, FC=0,5Hz E FC=10Hz
*/

/*  PINOS UTILIZADOS para a pulsaçao:
     A3 -> SINAL DO SENSOR, SAIDA DO AMP OP
     GND -> LIGAR A REFERENCIA DE TODO O CIRCUITO
   
   CONTADOR DE PASSOS
   MPU 6050 -> ACELEROMETRO GIROSCOPIO 3 EIXOS
   ENVIAR POR BLUETOOTH
   PINOS DO MPU:
   VCC -> 5V
   GND -> GND
   SCL -> A5
   SDA -> A4
   INT -> D2

   PINOS DO BLUETOOTH:
   STATE -> D4
   RX -> ARDUINO TX MAS COM DIVISOR DE TENSAO 3.3V
   TX -> ARDUINO RX
   GND -> GND
   VCC -> 5V (AGUENTA 3.3 A 6v)

   OS PINOS QUE NAO ESTAO INDICADOS DEVEM SER DEIXADOS DESLIGADOS

   PASSWORD DO BLUETOOTH: 1234
   BAUD RATE BLUETOOTH: 115200
*/
#include<Wire.h>
#include <SoftwareSerial.h>

//filtro
#define NZEROS 4
#define NPOLES 4
#define GAIN   1.587070257e+01
#define PIN_MODE 5 //pino do interruptor pra alternar entre modos
static float xv[NZEROS + 1], yv[NPOLES + 1];
static volatile unsigned long sensorValue = 0;        // will store ADC value from sensor
static volatile signed long long int outputValue = 0;
//para a zona dos tempos, alterar para colocar por interrupção?
unsigned long previousMillis = 0;        // will store last time LED was updated
//unsigned long tempo_anterior = 0;        // will store last time LED was updated
static volatile int bpm = 0;//numero de batimentos detetados por rising edge do filtro
static volatile long int bpm_calc = 0;//calculo dos batimentos por minuto
int flag_mode;

//pulsaçao+bluetooth:
const int MPU_addr = 0x68; // I2C address of the MPU-6050
volatile int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
static volatile int counter1 = 0, last = 0, counter2 = 0, counter3 = 0;
// BTconnected will = false when not connected and true when connected
boolean BTconnected = false;
// connect the STATE pin to Arduino pin D4
const byte BTpin = 4;
//SoftwareSerial BTserial(0, 1); // RX | TX
// Connect the HC-05 TX to Arduino pin 2 RX.
// Connect the HC-05 RX to Arduino pin 3 TX through a voltage divider(if its a 5V arduino).
char c = ' ';
static bool switch_pin_val;
//funcao do filtro digital implementado
/*
  static signed long long int filterloop()
  {
  xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
  xv[4] = (float)sensorValue / (float) GAIN;
  yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
  yv[4] = (xv[0] + xv[4]) - 2 * xv[2] + (-0.4311751778 * yv[0]) + (2.0252665529 * yv[1]) + (-3.7457226060 * yv[2]) + (3.1513614870 * yv[3]);
  outputValue = (long int) yv[4];
  return outputValue;
  }
*/

/*-------------------------------------------------------------------------FILTRO------------------------------------------------------------------------*/

signed long long int filterloop()
{
  xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
  xv[4] = (float)sensorValue / (float) GAIN;
  yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
  yv[4] = (xv[0] + xv[4]) - 2 * xv[2] + (-0.4311751778 * yv[0]) + (2.0252665529 * yv[1]) + (-3.7457226060 * yv[2]) + (3.1513614870 * yv[3]);
  outputValue = (long int) yv[4];
  return outputValue;
}

//calculo do tempo medio do batimento, de forma a estimar com mais precisão os batimentos por minuto
int calcular_media(long int vec[]) {
  float batimentos = (vec[4] - vec[3]) + (vec[3] - vec[2]) + (vec[2] - vec[1]) + (vec[1] - vec[0]);
  batimentos = batimentos / 4000; ///4 porque é a media de 4 pulsos e esta em milisegundos (1000) explica o pq de /(4*1000)
  return (int)(batimentos * 60);
}

//algoritmo de deteção de bpm, baseia-se na deteçao do rising edge do sinal filtrado, ao passar por zero, conta 1 batimento
void detect_bpm(long long int resultado) {
  unsigned long long int tempo = millis(), tempo1 = 0, tempo2 = 0;
  static long long int last_value = 0;
  static char flag = 0;
  static long int vec[5] = {0};
  static int i = 0;

  if (resultado == 0 || (resultado >= 0 && last_value < 0)) {//Se detetar rising edge ou a passar pelo zero, incrementa batimentos
    bpm = bpm + 1;
    vec[i++] = tempo;
  }
  if (i == 5) {// a cada 5 batimentos estima o periodo, calcula os bpms aproximados
    bpm_calc = calcular_media(vec);
    static long int bpm_anterior = 30;
    if (bpm_calc < 30) bpm_calc = bpm_anterior;
    else bpm_anterior = bpm_calc;
    i = 0;
  }
  last_value = resultado;
}

/*-------------------------------------------------------------------------PASSOS------------------------------------------------------------------------*/

//ROTINA DE CONTAGEM DOS PASSOS, MELHORAR ISTO PARA DETETAR MELHOR
void counterY()
{
  static bool flag = false;
  if (AcY >= -12000) flag = true;
  if (flag && AcY < -13000)
  {
    flag = false;
    counter1 += 1;
  }
}

void counterX()
{
  static bool flag = false;
  if (AcY >= -15000) flag = true;
  if (flag && AcY < -16000)
  {
    flag = false;
    counter2 += 1;
  }
}

void counterZ()
{
  static bool flag = false;
  if (AcY >= -15000) flag = true;
  if (flag && AcY < -16000)
  {
    flag = false;
    counter3 += 1;
  }
}
/*-------------------------------------------------------------------------setup e main loop-------------------------------------------------------------*/

//definir pinos como INPUT, OUTPUT, PORTA SERIE
void setup() {
  Serial.begin(115200);//velocidade da porta serie
  pinMode(PIN_MODE, INPUT); //definir pino do switch como input
  switch_pin_val = digitalRead(PIN_MODE);
  if(switch_pin_val){
    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
}
/*//test if the bluetooth is really connected
    Serial.println("Arduino is ready");
    Serial.println("Connect the HC-05 to an Android device to continue");
      // wait until the HC-05 has made a connection
      while (!BTconnected)
      {
        if ( digitalRead(BTpin)==HIGH)  { BTconnected = true;};
      }

      Serial.println("HC-05 is now connected");
      Serial.println("");
    //BTserial.begin(115200);
  */
}

//main loop
void loop() {
  //se for modo 0, ler pulsaçao, senao ler passos
  if (!switch_pin_val) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 10) {//a cada 10 ms fazer 1 amostragem do adc
      sensorValue = analogRead(A3);// It takes about 100 microseconds (0.0001 s) to read an analog input, so the maximum reading rate is about 10,000 times a second.
      previousMillis = currentMillis;
      signed long long int result = filterloop();//resultado do filtro, o filtro esta sempre a correr com os dados que vao chegando
      detect_bpm(result);

      //Serial.println(sensorValue);//valor lido diretamente do hardware
      Serial.println((double)result);//valor de saida do filtro digital
      //Serial.print(" ");//para conseguir ver varias linhas no serial plotter
      //Serial.println(bpm_calc);//ver calculo dos BPM
    }
  }
  else {
     Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);//VER ISTO
  Wire.requestFrom(MPU_addr, 7, true); // request a total of 14 registers -> VER ISTO
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  counterY();// CHAMAR A ROTINA DE CONTAR OS PASSOS
  counterX();// CHAMAR A ROTINA DE CONTAR OS PASSOS
  counterZ();// CHAMAR A ROTINA DE CONTAR OS PASSOS
  //ENVIAR POR BLUETOOTH OS DADOS
  //Serial.print(" | AcY = "); Serial.println(AcY);
  //Serial.print(" | AcX = "); Serial.println(AcX);
  //Serial.print(" | AcZ = "); Serial.println(AcZ);
  /*
    Serial.print(" | CounterY = "); Serial.println(counter1);
    Serial.print(" | CounterX = "); Serial.println(counter2);
    Serial.print(" | CounterZ = "); Serial.println(counter3);*/

  //to work with the android graph app
  Serial.print("E");
  Serial.print(AcX);
  Serial.print(",");
  Serial.print(Tmp);
  /*
  Serial.print(",");
  Serial.print(AcY);
  Serial.print(",");
  Serial.print(AcZ);
  */
  Serial.print("\n");
  delay(50);//ANALIZAR ESTE DELAY PARA ENTENDER A SUA EXISTENCIA
  }
}
