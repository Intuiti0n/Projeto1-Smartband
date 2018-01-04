/* PROJETO 1 SMART BAND, EEIC, 4ANO,
  SENSOR DE PULSAÇAO, ENVIAR POR PORTA SERIE
  AMOSTRAGEM 10 MS COM FILTRO (Fa=100Hz)
  FILTRO BUTTERWORTH 2A ORDEM, FC=0,5Hz E FC=10Hz
*/
/*  PINOS DO ECRA:
  VCC -> 3.3V
  GND->  GND
  SCL->  A5
  SDA->  A4

  ----------------------------------------------------------------
  -OS PINOS QUE NAO ESTAO INDICADOS DEVEM SER DEIXADOS DESLIGADOS-
  ----------------------------------------------------------------
  PASSWORD DO BLUETOOTH: 1234
  BAUD RATE BLUETOOTH: 115200 (foi alterado usando comandos AT, por defeito é 9600)

  VERIFICAR EXEMPLO DA BIBLIOTECA ADAFRUIT_SSD1306 164*32
*/
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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

//definir pinos como INPUT, OUTPUT, PORTA SERIE
void setup() {
  Serial.begin(115200);//velocidade da porta serie

  // DISPLAY
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  // internally, this will display the Adafruitsplashscreen.
  display.display();
  delay(2000);
  // Clear the buffer.
  display.clearDisplay();
}

//main loop
void loop() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("MODE:");
  //display.print(switch_pin_val);
  display.setCursor(0, 16);
  display.print("BPM:");
  //display.print(bpm_calc);
  display.display();
}
