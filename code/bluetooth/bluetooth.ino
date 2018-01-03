/* PROJETO 1 SMART BAND
    TESTE DO BLUETOOTH
    PINOS DO BLUETOOTH:
     STATE -> D4
     RX -> ARDUINO TX MAS COM DIVISOR DE TENSAO 3.3V
     TX -> ARDUINO RX
     GND -> GND
     VCC -> 5V (AGUENTA 3.3 A 6v)

     OS PINOS QUE NAO ESTAO INDICADOS DEVEM SER DEIXADOS DESLIGADOS
     PASSWORD DEFAULT: 1234
     BAUD RATE FOI ALTERADO PARA 115200 USANDO AT COMMANDS

     TIRAR OS PINOS DO TX E RX DO ARDUINO ANTES DE CARREGAR NOVO CODIGO PARA A PLACA OU VAO OCORRER ERROS DE SINCRONIZAÇÃO
*/
char c = ' ';

// BTconnected will = false when not connected and true when connected
boolean BTconnected = false;

// connect the STATE pin to Arduino pin D4
const byte BTpin = 4;


void setup()
{
  // set the BTpin for input
  pinMode(BTpin, INPUT);

  // start serial communication with the serial monitor on the host computer
  Serial.begin(115200);
  Serial.println("Arduino is ready");
  Serial.println("Connect the HC-05 to an Android device to continue");

  // wait until the HC-05 has made a connection
  while (!BTconnected)
  {
    if ( digitalRead(BTpin) == HIGH)  {
      BTconnected = true;
    };
  }

  Serial.println("HC-05 is now connected");
  Serial.println("");
}

void loop()
{
  // Keep reading from the HC-05 and send to Arduino Serial Monitor
  if (Serial.available())
  {
    c = Serial.read();
    Serial.write(c);
  }

  // Keep reading from Arduino Serial Monitor input field and send to HC-05
  if (Serial.available())
  {
    c =  Serial.read();
    Serial.write(c);
  }
}
