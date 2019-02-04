#include <Arduino.h>

//
#include <Unistep2.h>
// #define RPM_MAX               (18.0)
// #define STEPS_PER_REV         (4096.0)
// //
// // speed (rpm) * steps-per-revolution == speed (steps per minute)
// //  --> speed (steps per minute) / 60 == speed (steps per second)
// //  --> speed (steps per second) / 1000 == speed (steps per millisec)
// //  --> speed (steps per millisec) * 60 * 1000 / steps-per-revolution == speed (rpm)
// #define STEPS_PER_MILLISEC_TO_RPM (60.0 * 1000.0 / STEPS_PER_REV)
// //  --> speed (rpm) * steps-per-revolution / 60 / 1000 == speed (steps per millisec)
// #define RPM_TO_STEPS_PER_MILLISEC (STEPS_PER_REV / 60.0 / 1000.0)
// //  --> max. speed (rpm) --> max. speed (steps per millisec)
// #define STEPS_PER_MILLISEC_MAX (RPM_MAX * RPM_TO_STEPS_PER_MILLISEC)
// Stepper myStepper(STEPS_PER_REV, 8, 9, 10, 11);

//
Unistep2 stepper(8, 9, 10, 11, 4096, 1000);

//score list
static int score_now = 0;
static int note_count = 0;
static int score_steps[64] = { \
  1000, -2000, 3000, -2000, 1000, -3000, 10000, 1, };   // unit: steps
// static int step_seq_dur[64] = { \
//   100, 100, 100, 100, 100, 100, 100, 100, \
//   100, 100, 100, 100, 100, 100, 100, 100, \
//   100, 100, 100, 100, 100, 100, 100, 100, \
//   100, 100, 100, 100, 100, 100, 100, 100, \
//   100, 100, 100, 100, 100, 100, 100, 100, \
//   100, 100, 100, 100, 100, 100, 100, 100, \
//   100, 100, 100, 100, 100, 100, 100, 100, \
//   100, 100, 100, 100, 100, 100, 100, 100}; // unit: msec

//i2c
//pin connection
//nodemcu D1 <-> arduino nano A5 (SCL)
//nodemcu D2 <-> arduino nano A4 (SDA)
#include <Wire.h>
#include "../i2c_protocol.h"
void receiveEvent(int numBytes) {
  //numBytes : how many bytes received(==available)
  int nb = Wire.readBytes(cmdstr, CMD_LENGTH);
  Serial.print("[i2c] cmdstr : ");
  Serial.println(cmdstr);
  if (CMD_LENGTH == nb) {
    String msg = String(cmdstr);

    //
    // "P#SS-/-/-/" - P: P (play), SS: song #
    // "S-/-/-/-/-" - S: S (stop)
    //

    //parse command string.
    char first = msg.charAt(0);
    if (first == 'P') {
      //
      // "P#SS-/-/-/" - P: P (play), SS: song #
      //
      String str_music = msg.substring(2,4); // 23
      score_now = str_music.toInt();
      //
      // sound_player_start_task.restart();
      //
    } else if (first == 'S') {
      //
      // "S-/-/-/-/-" - S: S (stop)
      //
      // sound_player_stop_task.restart();
      //
    } else {
      // error. unknown command
    }
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
  // myStepper.setSpeed(speed * STEPS_PER_MILLISEC_TO_RPM); //rpm
  // myStepper.step(target);

  stepper.run();
  if ( stepper.stepsToGo() == 0 ) {
    stepper.move(score_steps[note_count]);
    Serial.println(score_steps[note_count]);
    note_count++;
    if (note_count >= 64) {
      note_count = 0;
    }
  }
}



//
// static int count = 0;
// static int step_pos_prev = 0;
// static char cstr[256] = {0, };
// if (count < 64) {
//   //
//   // sanity check (speed)
//   //
//   float steps_to_move = step_seq_pos[count] - step_pos_prev;
//   float moving_speed = steps_to_move / step_seq_dur[count]; // unit: (steps/msec)
//   if (steps_to_move != 0) {
//     if (fabs(moving_speed) > STEPS_PER_MILLISEC_MAX) {
//       //oh.. that might be TOO fast, isn't it??
//       Serial.println("oh.. isn't it TOO FAST??");
//     } else {
//       //my next move will be this fast.. and i can do that.
//       Serial.println("okay. i go now.");
//     }
//   }
//
//   //DEBUG
//   Serial.println("[SEQ]");
//   Serial.print  ("  count: "); Serial.println(count);
//   Serial.print  ("  step_seq_pos: "); Serial.println(step_seq_pos[count]);
//   Serial.print  ("  step_pos_prev: "); Serial.println(step_pos_prev);
//   Serial.print  ("  steps_to_move: "); Serial.println(steps_to_move);
//   Serial.print  ("  moving_spd: "); Serial.print(moving_speed); Serial.println(" (steps/msec)");
//   Serial.print  ("  max_spd: "); Serial.print(STEPS_PER_MILLISEC_MAX); Serial.println(" (steps/msec)");
//
//
//   //
//   // "P#SS-/-/-/" - P: P (play), SS: song #
//   // "S-/-/-/-/-" - S: S (stop)
//   //
//
