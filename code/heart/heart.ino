/* PROJETO 1 SMART BAND
 *  SENSOR DE PULSAÇAO, ENVIAR POR PORTA SERIE
    TESTE AMOSTRAGEM 10 MS COM FILTRO
    FILTRO BUTTERWORTH 2A ORDEM, FC=0,5Hz E FC=10Hz
    Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
*/

/* PODE DAR JEITO
  // defines for setting and clearing register bits
  #ifndef cbi
  #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
  #endif
  #ifndef sbi
  #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
  #endif
*/

/*  PINOS UTILIZADOS DO ARDUINO:
 *   A0 -> SINAL DO SENSOR, SAIDA DO AMP OP
 *   GND -> LIGAR A REFERENCIA DE TODO O CIRCUITO
 */
 
//filtro
#define NZEROS 4
#define NPOLES 4
#define GAIN   1.587070257e+01
static float xv[NZEROS + 1], yv[NPOLES + 1];
static volatile unsigned long sensorValue = 0;        // will store ADC value from sensor
static volatile signed long long int outputValue = 0;

//para a zona dos tempos, alterar para colocar por interrupção?
unsigned long previousMillis = 0;        // will store last time LED was updated
//unsigned long tempo_anterior = 0;        // will store last time LED was updated


static volatile int bpm = 0;//numero de batimentos detetados por rising edge do filtro
static volatile long int bpm_calc = 0;//calculo dos batimentos por minuto

//funcao do filtro digital implementado
static signed long long int filterloop()
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


void setup() {
  Serial.begin(115200);//velocidade da porta serie
}

void loop() {
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

