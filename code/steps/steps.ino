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
volatile int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
static volatile int counter1 = 0, last = 0, counter2 = 0, counter3 = 0;

// BTconnected will = false when not connected and true when connected
boolean BTconnected = false;

// connect the STATE pin to Arduino pin D4
const byte BTpin = 4;


//SoftwareSerial BTserial(0, 1); // RX | TX
// Connect the HC-05 TX to Arduino pin 2 RX.
// Connect the HC-05 RX to Arduino pin 3 TX through a voltage divider.

char c = ' ';

void setup() {
  Wire.setClock(100000);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(115200);
  /*
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


void loop() {
  long int mils=millis();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);//VER ISTO
  Wire.requestFrom(MPU_addr, 3, true); // request a total of 14 registers -> VER ISTO
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
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

  //to work with the android app
  Serial.print("E");
  Serial.print(AcX);
  /*
  Serial.print(",");
  Serial.print(AcY);
  Serial.print(",");
  Serial.print(AcZ);
  */
  Serial.print("\n");
  long mils2=millis();
  Serial.println(mils);
  Serial.println(mils2);
  delay(1000);//ANALIZAR ESTE DELAY PARA ENTENDER A SUA EXISTENCIA
}
