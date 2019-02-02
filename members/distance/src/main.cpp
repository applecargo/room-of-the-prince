#include <Arduino.h>

//
static int dist_cm; //cm

//
// "D000XXXXXX" - D: distance(cm)
//

//i2c
//pin connection
//nodemcu D1 <-> arduino nano A5 (SCL)
//nodemcu D2 <-> arduino nano A4 (SDA)
#include <Wire.h>
#include "../i2c_protocol.h"
// void receiveEvent(int numBytes) {
// }
void requestEvent() {
  if (dist_cm > 999) {
    dist_cm = 999;
  }
  sprintf(cmdstr, "D%03d-/-/-/", dist_cm);
  Wire.write(cmdstr);
}

//
const int trig = 9;
const int echo = 8;
const int led = 13;
//
void setup() {
  //
  Serial.begin(9600);

  //i2c
  Wire.begin(I2C_ADDR);
  // Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //trigger -> OUTPUT
  //echo -> INPUT
  //led -> OUTPUT
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  //
  digitalWrite(trig, LOW);
  digitalWrite(echo, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  unsigned long duration = pulseIn(echo, HIGH);
  float distance = duration / 29.0 / 2.0;

  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println("cm");

  //
  dist_cm = distance;

  //
  delay(200);
}
