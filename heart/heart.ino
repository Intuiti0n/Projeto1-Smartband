unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 50;           // interval at which to blink (milliseconds)

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 10) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // set the LED with the ledState of the variable:
    int sensorValue=analogRead(A0);
    Serial.println(sensorValue);
  }
}
