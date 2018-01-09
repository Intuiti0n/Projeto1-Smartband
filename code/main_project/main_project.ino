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

  PINOS DO ECRA:
  VCC -> 3.3V
  GND->  GND
  SCL->  A5
  SDA->  A4

  ----------------------------------------------------------------
  -OS PINOS QUE NAO ESTAO INDICADOS DEVEM SER DEIXADOS DESLIGADOS-
  ----------------------------------------------------------------
  PASSWORD DO BLUETOOTH: 1234
  BAUD RATE BLUETOOTH: 115200 (foi alterado usando comandos AT, por defeito é 9600)
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PIN_MODE 5 //pino do interruptor pra alternar entre modos
#define PIN_HEART A3
//filtro pulsaçao
#define NZEROS 4
#define NPOLES 4
#define GAIN   1.587070257e+01
static float xv[NZEROS + 1], yv[NPOLES + 1];
static volatile unsigned long sensorValue = 0;        // will store ADC value from sensor
static volatile signed long long int outputValue = 0;
unsigned long previousMillis = 0;        // pulsaçao
static volatile int bpm = 0;//numero de batimentos detetados por rising edge do filtro
static volatile long int bpm_calc = 0;//calculo dos batimentos por minuto

//steps
unsigned long tempo_anterior = 0;        // passos
#define STEP_THRESHOLD 400 //threshold do sensor de passos, do algoritmo basico
const int MPU_addr = 0x68; // I2C address of the MPU-6050
volatile int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
static volatile int counter1 = 0, last = 0, counter2 = 0, counter3 = 0;
static long int step_counter = 0;//count the steps

//bluetooth
// BTconnected will = false when not connected and true when connected
boolean BTconnected = false;
const byte BTpin = 4;//pin to test the STATE of the bluetooth
static bool switch_pin_val;//pin to change between steps and heart rate monitor
/*-------------------------------------------------------------------------OLED------------------------------------------------------------------------*/

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};

//testar se estamos a utilizar o ecra correto
#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

/*-------------------------------------------------------------------------FILTRO HEART------------------------------------------------------------------------*/
//funcao do filtro digital implementado para o sensor de bpm cardiacos
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
  long int batimentos = abs(vec[4] - vec[3]) + abs(vec[3] - vec[2]) + abs(vec[2] - vec[1]) + abs(vec[1] - vec[0]);
  for(int i=0;i<5;i++){
    Serial.println(abs(vec[i]));
  }
  batimentos = batimentos / 4000; ///4 porque é a media de 4 pulsos e esta em milisegundos (1000) explica o pq de /(4*1000)
   Serial.print(batimentos*60);
   Serial.print("\n\n");
  return (int)(batimentos * 60);
}

//algoritmo de deteção de bpm, baseia-se na deteçao do rising edge do sinal filtrado, ao passar por zero, conta 1 batimento
void detect_bpm(long long int resultado) {
  unsigned long int tempo = millis(), tempo1 = 0, tempo2 = 0;
  static long int last_value = 0;
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
void steps(int value) {
  static char flag = 0;
  if (value > STEP_THRESHOLD && flag == 0) {
    flag = 1;
    step_counter++;
  }

  if (flag == 1 && value < 0) {
    flag = 0;
  }
}

//valores para o filtro dos passos
#define STEP_NZEROS 4
#define STEP_NPOLES 4
#define STEP_GAIN   5.950605930e+01

static float STEP_xv[STEP_NZEROS + 1], STEP_yv[STEP_NPOLES + 1];

//rotina do filtro de passos
static float STEP_filterloop(float in) {
  static float out = 0;
  STEP_xv[0] = STEP_xv[1]; STEP_xv[1] = STEP_xv[2]; STEP_xv[2] = STEP_xv[3]; STEP_xv[3] = STEP_xv[4];
  STEP_xv[4] = in / STEP_GAIN;
  STEP_yv[0] = STEP_yv[1]; STEP_yv[1] = STEP_yv[2]; STEP_yv[2] = STEP_yv[3]; STEP_yv[3] = STEP_yv[4];
  STEP_yv[4] =   (STEP_xv[0] + STEP_xv[4]) - 2 * STEP_xv[2]
                 + ( -0.6704579059 * STEP_yv[0]) + (  2.9304272168 * STEP_yv[1])
                 + ( -4.8462898043 * STEP_yv[2]) + (  3.5862398081 * STEP_yv[3]);
  out = STEP_yv[4];
  return out;
}
/*-------------------------------------------------------------------------setup e main loop-------------------------------------------------------------*/

//definir pinos como INPUT, OUTPUT, PORTA SERIE
void setup() {
  Serial.begin(115200);//velocidade da porta serie
  pinMode(PIN_MODE, INPUT); //definir pino do switch como input

  // DISPLAY
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // internally, this will display the Adafruitsplashscreen.
  display.display();//use this after each change to the screen
  delay(2000);
  // Clear the buffer.
  display.clearDisplay();

  //se PIN = ON -> MODO STEPS, SE OFF, MODO PULSAÇAO
  switch_pin_val = digitalRead(PIN_MODE);//ler pino do modo no inicio do programa
  if (switch_pin_val) {
    Wire.setClock(100000);//I2C A 100kHz
    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
  }
  /*
  // wait until the HC-05 has made a connection
  while (!BTconnected)
  {
    if ( digitalRead(BTpin) == HIGH)  {
      BTconnected = true;
    };
  }*/
}

//main loop
void loop() {
  //se for modo 0, ler pulsaçao, senao ler passos
  //PULSACAO
  if (!switch_pin_val) {
    display.clearDisplay();
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 10) {//a cada 10 ms fazer 1 amostragem do adc
      sensorValue = analogRead(PIN_HEART);// It takes about 100 microseconds (0.0001 s) to read an analog input, so the maximum reading rate is about 10,000 times a second.
      previousMillis = currentMillis;
      signed long long int result = filterloop();//resultado do filtro, o filtro esta sempre a correr com os dados que vao chegando
      detect_bpm(result);
      
      //to work with the android graph app
      //Serial.print("E");
      //Serial.print(sensorValue);//valor lido diretamente do hardware
      //Serial.print(",");//para conseguir ver varias linhas no serial plotter
      //Serial.print((double)result);//valor de saida do filtro digital
      //Serial.print(",");//para conseguir ver varias linhas no serial plotter
      //Serial.print(bpm_calc);//ver calculo dos BPM
      //Serial.print("\n");

      //DISPLAY
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("MODE:");
      display.print(switch_pin_val);
      display.setCursor(0, 16);
      display.print("BPM:");
      display.print(bpm_calc);
      display.display();
    }
  }
  //STEPS
  else {
    int8_t trash = 0;
    unsigned long tempo_atual = millis();
    if (tempo_atual - tempo_anterior >= 10) {
      tempo_anterior = tempo_atual;
      Wire.beginTransmission(MPU_addr);
      Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
      Wire.endTransmission(false);//VER ISTO
      Wire.requestFrom(MPU_addr, 5, true); // request a total of 14 registers -> 2 * 8 bit registers
      AcX = Wire.read();// 0x3B (ACCEL_XOUT_H)
      trash = Wire.read(); // & 0x3C (ACCEL_XOUT_L)
      AcY = Wire.read() ; // 0x3D (ACCEL_YOUT_H)
      trash = Wire.read(); //0x3E (ACCEL_YOUT_L)
      AcZ = Wire.read() ; // 0x3F (ACCEL_ZOUT_H)

      float filtro = STEP_filterloop(AcY * AcZ);
      steps(filtro);

      Serial.print("E");
      Serial.print((int) filtro);
      Serial.print(",");
      Serial.println(step_counter);

      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.print("MODE:");
      display.print(switch_pin_val);
      display.setCursor(0, 16);
      //display.print("Temp:");
      //display.print(float(Tmp) / 340 + 36.53);
      display.print("STEP:");
      display.print(step_counter);
      display.display();
    }
  }
}
