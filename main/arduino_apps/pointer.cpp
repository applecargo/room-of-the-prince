void setup()
{
  // led indicator
  pinMode(LED_BUILTIN, OUTPUT);

  // load vocabulary
  strcpy(voca[0], "0Hello, I'm a pointer motor, called Chin Cheng.");
  strcpy(voca[1], "1Gonniziwa!!!");
  strcpy(voca[2], "2Hazime mashite.");
  strcpy(voca[3], "3O-nakka hae tae i masu?");
  strcpy(voca[4], "4Nan-ka tabetara?");
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
