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

    Wire.beginTransmission(MPU_addr);//a cada 10 ms fazer 1 amostragem do adc
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);//VER ISTO
    Wire.requestFrom(MPU_addr, 14, true); // 2* numero de registos (sao de 16 bits)
    AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)


     PINOS DO BLUETOOTH:
     STATE -> D4
     RX -> ARDUINO TX MAS COM DIVISOR DE TENSAO 3.3V
     TX -> ARDUINO RX
     GND -> GND
     VCC -> 5V (AGUENTA 3.3 A 6v)

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

// Connect the HC-05 TX to Arduino pin 2 RX.
// Connect the HC-05 RX to Arduino pin 3 TX through a voltage divider(if you are using a 5v arduino).
/*
  OS PINOS QUE NAO ESTAO INDICADOS DEVEM SER DEIXADOS DESLIGADOS

  PASSWORD DO BLUETOOTH: 1234
  BAUD RATE BLUETOOTH: 115200
*/


#include<Wire.h>

#define STEP_THRESHOLD 400
const int MPU_addr = 0x68; // I2C address of the MPU-6050
static volatile int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
static volatile int counter1 = 0, last = 0, counter2 = 0, counter3 = 0;

//para a zona dos tempos, alterar para colocar por interrupção?
unsigned long previousMillis = 0;        // will store last time LED was updated
//unsigned long tempo_anterior = 0;        // will store last time LED was updated


// BTconnected will = false when not connected and true when connected
boolean BTconnected = false;
// connect the STATE pin to Arduino pin D4
const byte BTpin = 4;
static long int step_counter=0;

void setup() {
  Wire.setClock(100000);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(115200);

  // wait until the HC-05 has made a connection
  while (!BTconnected)
  {
    if ( digitalRead(BTpin) == HIGH)  {
      BTconnected = true;
    };
  }
}

//ordem 2, 5Hz
/*
  #define NZEROS 2
  #define NPOLES 2
  #define GAIN   4.979245121e+01

  static float xv[NZEROS + 1], yv[NPOLES + 1];
  /*
  static float filterloop(float in) {
  static float out = 0;
  xv[0] = xv[1]; xv[1] = xv[2];
  xv[2] = in / GAIN;
  yv[0] = yv[1]; yv[1] = yv[2];
  yv[2] =   (xv[0] + xv[2]) + 2 * xv[1]+ ( -0.6413515381 * yv[0]) + (  1.5610180758 * yv[1]);
  out = yv[2];
  return out;
  }
*/
void steps(int value){
  static char flag=0;
  if(value > STEP_THRESHOLD&& flag==0){
    flag=1;
    step_counter++;
  }

  if(flag==1 && value < 0){
    flag=0;
  }
}

#define NZEROS 4
#define NPOLES 4
#define GAIN   5.950605930e+01

static float xv[NZEROS + 1], yv[NPOLES + 1];

static float filterloop(float in) {
  static float out = 0;
  xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
  xv[4] = in / GAIN;
  yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
  yv[4] =   (xv[0] + xv[4]) - 2 * xv[2]
            + ( -0.6704579059 * yv[0]) + (  2.9304272168 * yv[1])
            + ( -4.8462898043 * yv[2]) + (  3.5862398081 * yv[3]);
  out = yv[4];
  return out;
}

void loop() {
  static float teste;
  int8_t trash = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 10) {
    previousMillis = currentMillis;
    Wire.beginTransmission(MPU_addr);//a cada 10 ms fazer 1 amostragem do adc
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);//VER ISTO
    Wire.requestFrom(MPU_addr, 5, true); // request a total of 14 registers -> VER ISTO
    AcX = Wire.read();// 0x3B (ACCEL_XOUT_H)
    trash = Wire.read(); // & 0x3C (ACCEL_XOUT_L)
    AcY = Wire.read() ; // 0x3D (ACCEL_YOUT_H)
    trash = Wire.read(); //0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() ; // 0x3F (ACCEL_ZOUT_H)
    //trash = Wire.read(); //0x40 (ACCEL_ZOUT_L)


    //revolta
    //ler apenas os 8 bits mais significativos
    /*
      AcX = Wire.read() >> 2;// 0x3B (ACCEL_XOUT_H)
      trash = Wire.read(); // & 0x3C (ACCEL_XOUT_L)
      AcY = Wire.read() >> 2; // 0x3D (ACCEL_YOUT_H)
      trash = Wire.read(); //0x3E (ACCEL_YOUT_L)
      AcZ = Wire.read() >> 2; // 0x3F (ACCEL_ZOUT_H)
      trash = Wire.read(); //0x40 (ACCEL_ZOUT_L)
      Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    */
    //AcX/=1000;
    //AcY/=1000;
    //AcZ/=1000;
    float acc_total_vector = sqrt(((float)AcX * AcX) + ((float)AcY * AcY) + ((float)AcZ * AcZ));
    float filtro = filterloop(AcY * AcZ);
    /*
      //teste = filterloop((float)AcX);
      Serial.println((int)acc_total_vector);
      Serial.print("AcX = "); Serial.print(AcX);
      Serial.print(" | AcY = "); Serial.print(AcY);
      Serial.print(" | AcZ = "); Serial.print(AcZ);
      Serial.print("\n");
    */
    /*
      Serial.print("E");
      Serial.print(AcX);
      Serial.print(",");
      Serial.print(AcY);
      Serial.print(",");
      Serial.println(AcZ);
    */
    steps(filtro);
    Serial.print("E");
    //Serial.print(filtro);
    //Serial.print(",");
    Serial.println(step_counter);
    //Serial.print(",");
    //Serial.println(filtro);
    //Serial.println(filtro);
    //Serial.print("\n");
  }
  //ENVIAR POR BLUETOOTH OS DADOS
  //to work with the android app
  //para imprimir eixos e temperatura
  /*
    Serial.print("E");
    Serial.print(AcX);
    Serial.print(",");
    Serial.print(float(Tmp)/340+36.53);  //equation for temperature in degrees C from datasheet
  */
  //erial.print("E");
  /* Serial.print(AcX);
    Serial.print(" ");
    Serial.print(AcY);
    Serial.print(" ");
    Serial.print(AcZ);
    //Serial.print(float(Tmp)/340+36.53);  //equation for temperature in degrees C from datasheet
    Serial.print("\n");*/
}


/* ler todos os registos do MPU:

    Serial.print("AcX = "); Serial.print(AcX);
    Serial.print(" | AcY = "); Serial.print(AcY);
    Serial.print(" | AcZ = "); Serial.print(AcZ);
    Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
    Serial.print(" | GyX = "); Serial.print(GyX);
    Serial.print(" | GyY = "); Serial.print(GyY);
    Serial.print(" | GyZ = "); Serial.println(GyZ);
    delay(333);*/
