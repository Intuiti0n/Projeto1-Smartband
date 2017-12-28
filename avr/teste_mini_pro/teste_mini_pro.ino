#define PIN 12
#define adc A6
void setup() {
  pinMode(PIN, OUTPUT);
  Serial.begin(115200);
}


void loop() {
  digitalWrite(PIN, HIGH);
  int adc_value = analogRead(A3);
  Serial.println(adc_value);
  delay(500);
}
