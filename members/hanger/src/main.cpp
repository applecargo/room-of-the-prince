#include <Arduino.h>
#include <Audio.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
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

#define analogPinForRV    1
#define analogPinForTMP   0

AudioSynthWaveformSine sine1;            //xy=159,201
AudioOutputAnalog dac1;                  //xy=373,211
AudioConnection patchCord1(sine1, dac1);
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

  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 20) {

    // get wind data...
    float windspeed = getWindSpeed();
    Serial.print("windspeed => ");
    Serial.println(windspeed);

    sine1.amplitude(0.3);
    sine1.frequency(windspeed*10);
    //
    lastMillis = millis();
  }



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

float getWindSpeed() {
  //parameter
  const float zeroWindAdjustment =  .2;
  //
  static int TMP_Therm_ADunits = 0;
  static float RV_Wind_ADunits = 0;
  static float RV_Wind_Volts = 0;
  static int TempCtimes100 = 0;
  static float zeroWind_ADunits = 0;
  static float zeroWind_volts = 0;
  static float WindSpeed_MPH = 0;

  TMP_Therm_ADunits = analogRead(analogPinForTMP);
  RV_Wind_ADunits = analogRead(analogPinForRV);
  RV_Wind_Volts = (RV_Wind_ADunits *  0.0048828125);
  TempCtimes100 = (0.005 * ((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits)) - (16.862 * (float)TMP_Therm_ADunits) + 9075.4;
  zeroWind_ADunits = -0.0006 * ((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits) + 1.0727 * (float)TMP_Therm_ADunits + 47.172; //  13.0C  553  482.39
  zeroWind_volts = (zeroWind_ADunits * 0.0048828125) - zeroWindAdjustment;

  return pow(((RV_Wind_Volts - zeroWind_volts) / .2300), 2.7265);
}
