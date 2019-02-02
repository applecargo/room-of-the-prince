#include <Arduino.h>

//
#include <Stepper.h>
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);
int stepCount = 0;

//
// "STEP%04d-/" - STEP: target step
//

//i2c
//pin connection
//nodemcu D1 <-> arduino nano A5 (SCL)
//nodemcu D2 <-> arduino nano A4 (SDA)
#include <Wire.h>
#include "../i2c_protocol.h"
void receiveEvent(int numBytes) {
  //numBytes : how many bytes received(==available)

  // Serial.println("[i2c] on receive!");
  int nb = Wire.readBytes(cmdstr, CMD_LENGTH);
  Serial.print("[i2c] cmdstr : ");
  Serial.println(cmdstr);
  // Serial.print("[i2c] nb : ");
  // Serial.println(nb);

  if (CMD_LENGTH == nb) { // receive cmdstr.
    //convert to String
    String msg = String(cmdstr);

    //
    // "STEP%04d-/" - STEP: target step
    //

    //parse command string.
    String target = msg.substring(4,8); // 4567

    //step target
    Serial.print("step target:");
    Serial.println(target);

    //move stepper
    myStepper.step(target);
    stepCount += target;
  }
}

//
void setup() {
  //
  Serial.begin(9600);

  //i2c
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receiveEvent);

  //stepper pins
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
}

void loop() {
  ;
}
