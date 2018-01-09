//codigo usado pra testar todos os pinos do ARDUINO MINI PRO
#define PIN 12
#define adc A6
void setup() {
  pinMode(PIN, OUTPUT);
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  
}


void loop() {
  digitalWrite(PIN, HIGH);
  digitalWrite(13,HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  int adc_value = analogRead(A2);
  Serial.println(adc_value);
  delay(50);
}
