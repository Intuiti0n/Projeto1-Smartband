/*  PROJETO 1 SMART BAND
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
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
static volatile int counter1 = 0, last = 0;

// BTconnected will = false when not connected and true when connected
boolean BTconnected = false;
 
// connect the STATE pin to Arduino pin D4
const byte BTpin = 4;
 

//SoftwareSerial BTserial(0, 1); // RX | TX
// Connect the HC-05 TX to Arduino pin 2 RX.
// Connect the HC-05 RX to Arduino pin 3 TX through a voltage divider.

char c = ' ';

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(115200);

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
}

//ROTINA DE CONTAGEM DOS PASSOS, MELHORAR ISTO PARA DETETAR MELHOR
void counter()
{
  static bool flag = false;
  if (AcY >= -15000) flag = true;
  if (flag && AcY < -16000)
  {
    flag = false;
    counter1 += 1;
  }
}


void loop() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);//VER ISTO
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers -> VER ISTO
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  counter();// CHAMAR A ROTINA DE CONTAR OS PASSOS

  //ENVIAR POR BLUETOOTH OS DADOS
  Serial.print(" | AcY = "); Serial.println(AcY);
  Serial.print(" | Counter = "); Serial.println(counter1);
  delay(333);//ANALIZAR ESTE DELAY PARA ENTENDER A SUA EXISTENCIA
}
