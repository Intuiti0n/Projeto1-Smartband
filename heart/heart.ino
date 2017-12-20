/* DANIEL GOMES A74729
 *  TESTE AMOSTRAGEM 10 MS COM FILTRO
 *  FILTRO BUTTERWORTH 2A ORDEM, FC=0,5Hz E FC=10Hz
/ *  Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher*/

#define NZEROS 4
#define NPOLES 4
#define GAIN   1.587070257e+01

unsigned long previousMillis = 0;        // will store last time LED was updated
//unsigned long tempo_anterior = 0;        // will store last time LED was updated
static float xv[NZEROS + 1], yv[NPOLES + 1];
static volatile unsigned long sensorValue = 0;        // will store ADC value from sensor
static volatile signed long long int outputValue=0;
static volatile int bpm=0;
static volatile long int bpm_calc=0;
static signed long long int filterloop()
{
    xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4];
    xv[4] = (float)sensorValue /(float) GAIN;
    yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4];
    yv[4] = (xv[0] + xv[4]) - 2 * xv[2]+ (-0.4311751778 * yv[0]) + (2.0252665529 * yv[1])+ (-3.7457226060 * yv[2]) + (3.1513614870 * yv[3]);
    outputValue =(long int) yv[4];
    return outputValue;
}

int calcular_media(long int vec[]){
  //float batimentos= vec[0]+vec[1]+vec[2]+vec[3]+vec[4];
  float batimentos= (vec[4]-vec[3])+(vec[3]-vec[2])+(vec[2]-vec[1])+(vec[1]-vec[0]);
  batimentos=batimentos/4000;
  return (int)(batimentos*60);
}
void detect_bpm(long long int resultado){
  unsigned long long int tempo=millis(),tempo1=0,tempo2=0;
  static long long int last_value=0;
  static char flag=0;
  static long int vec[5]={0};
  static int i=0;
  if(resultado==0||(resultado>=0&&last_value<0)){
    bpm=bpm+1;
    vec[i++]=tempo;
  }
  if(i==5){
    bpm_calc=calcular_media(vec);
    i=0;
  }
    last_value=resultado;
}


void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 10) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // set the LED with the ledState of the variable:
    sensorValue=analogRead(A0);
    //Serial.println(sensorValue);
    signed long long int result=filterloop();
    detect_bpm(result);

    Serial.println(sensorValue);
    //Serial.println((double)result);
    //Serial.print(" ");
    //Serial.println(bpm_calc);
    
  }
}

