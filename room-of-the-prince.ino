void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH); //OFF
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW); // ON
  delay(10);
}
