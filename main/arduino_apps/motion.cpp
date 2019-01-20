void setup()
{
  // led indicator
  pinMode(LED_BUILTIN, OUTPUT);

  // load vocabulary
  strcpy(voca[0], "0Hello, I'm a motion detector, called Meisenstroofpen.");
  strcpy(voca[1], "1Hey! There's something moving!!!");
  strcpy(voca[2], "2Well.. nothing is moving. How come?");
  strcpy(voca[3], "3Seriously, I want to know what was the thing right now? any idea?");
  strcpy(voca[4], "4Could you be more stay still for me? I cannot concentrate!");
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
