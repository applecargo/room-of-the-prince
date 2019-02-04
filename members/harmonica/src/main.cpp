#include <Arduino.h>

//
#include "../metrics.h"
#include <Stepper.h>
Stepper myStepper(STEPS_PER_REV, 8, 9, 10, 11);
int stepCount = 0;
float target = 0; //steps
float duration = 1000; //msec
// bool new_move = false;

//
// "PppppDdddd"
//   'P' - target position (steps)
//   'D' - movement duration (msec)
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
    // "PppppDdddd"
    //   'P' - target position (steps)
    //   'D' - movement duration (msec)
    //

    //parse command string.
    String str_position = msg.substring(1,5); // 1234
    String str_duration = msg.substring(6,10); // 6789
    target = str_position.toFloat();
    duration = str_duration.toFloat();
    // new_move = true;

    // //step target
    // Serial.print("target:");
    // Serial.println(target);
    // Serial.print("duration:");
    // Serial.println(duration);
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
  //move stepper
  float speed = target / duration; //steps per millisec.
  myStepper.setSpeed(speed * STEPS_PER_MILLISEC_TO_RPM); //rpm
  Serial.println(speed * STEPS_PER_MILLISEC_TO_RPM);
  myStepper.step(target);
  stepCount += target;
}
