void setup()
{
  // led indicator
  pinMode(LED_BUILTIN, OUTPUT);

  // load vocabulary
  strcpy(voca[0], "0Yo, I'm a relay controller, called Diego Larambla.");
  strcpy(voca[1], "1What! You mean there's a person?");
  strcpy(voca[2], "2Tick, tack, toe--.");
  strcpy(voca[3], "3I don't know either, amigo.");
  strcpy(voca[4], "4Ya well, I do sincerely apologize, amigo.");
}

void loop()
{
  // ESP_LOGI(MESH_TAG, "loop()");

  //simple blink test-code.
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  vTaskDelay(10 / portTICK_RATE_MS);
}
