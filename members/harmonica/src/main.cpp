#include <Arduino.h>

//relay
#define RELAY_PIN 5

//
#include <CheapStepper.h>
#define RPM_MAX               (18.0)
#define STEPS_PER_REV         (4096.0)
//
// speed (rpm) * steps-per-revolution == speed (steps per minute)
//  --> speed (steps per minute) / 60 == speed (steps per second)
//  --> speed (steps per second) / 1000 == speed (steps per millisec)
//  --> speed (steps per millisec) * 60 * 1000 / steps-per-revolution == speed (rpm)
#define STEPS_PER_MILLISEC_TO_RPM (60.0 * 1000.0 / STEPS_PER_REV)
//  --> speed (rpm) * steps-per-revolution / 60 / 1000 == speed (steps per millisec)
#define RPM_TO_STEPS_PER_MILLISEC (STEPS_PER_REV / 60.0 / 1000.0)
//  --> max. speed (rpm) --> max. speed (steps per millisec)
#define STEPS_PER_MILLISEC_MAX (RPM_MAX * RPM_TO_STEPS_PER_MILLISEC)

//
CheapStepper stepper (8,9,10,11);

//score list
static int score_now = 0;
//score #0 reserved as a RANDOM play.
#define SCORE_NOTE_MAX 8
#define SCORE_COUNT 2
//score #1
static int note_idx = 0;
static int note_steps[SCORE_COUNT][SCORE_NOTE_MAX] = { //unit: steps
  {  1000,  2000,  3000,  2000,  1000,  3000, 10000,     1},  // score #1
  {  3000,  2000, 10000,  2500,  1000,  1000,   100,   400}   // score #2
};
static int note_dur[SCORE_COUNT][SCORE_NOTE_MAX] = { // unit: msec
  {   100,   100,   100,   100,   100,   100,     0,     0},  // score #1
  {   100,   100,   100,   100,   100,   100,     0,     0}   // score #2
};

//task
#include <TaskScheduler.h>
Scheduler runner;
extern Task music_player_stepping_task;
extern Task music_player_stop_task;
bool is_music_time = false;
//
void music_player_stepping() {
  //
  if (stepper.getStepsLeft() == 0 && is_music_time == true) {
    //
    digitalWrite(RELAY_PIN, HIGH); // blow start! (and continue.)
    //
    float cur_step = stepper.getStep();
    float target_step = note_steps[score_now][note_idx];
    float dur = note_dur[score_now][note_idx];
    float steps = target_step - cur_step;
    float rpm = fabs(steps / dur * STEPS_PER_MILLISEC_TO_RPM); // unit conv.: (steps/msec) --> (rpm)
    //
    if (rpm > RPM_MAX) {
      Serial.println("oh.. isn't it TOO FAST??");
    } else {
      Serial.println("okay. i go now.");
    }
    //
    Serial.print(" --> speed(rpm) : "); Serial.println(rpm);
    //
    stepper.setRpm(rpm);
    stepper.newMoveTo(steps > 0, target_step); //first arg. : "CW or CCW?"
    //
    note_idx++;
    //
    if (note_idx >= SCORE_NOTE_MAX) { //end of the song.
      // rewind the reel.
      note_idx = 0;
      // blow stop!
      digitalWrite(RELAY_PIN, LOW);
      // schedule a stopping task!
      music_player_stop_task.restart();
    } else {
      // reschedule myself..
      music_player_stepping_task.restartDelayed(dur);
    }
  } else {
    // reschedule myself..
    music_player_stepping_task.restartDelayed(100);
    //
    Serial.println("stepper BUSY! will wait a bit.");
  }
}
Task music_player_stepping_task(0, TASK_ONCE, music_player_stepping);

//
void music_player_stop() {
  if (music_player_stop_task.isFirstIteration()) {
    //block 'music_player_stepping_task'
    is_music_time = false;
    // stay still for sometime! (while blowing stops..)
    stepper.newMoveTo(true, stepper.getStep()); //first arg. : "CW or CCW?"
    // blow stop!
    digitalWrite(RELAY_PIN, LOW);
  } else {
    //okay, blower is over, now. then, go home position.
    stepper.setRpm(20);
    stepper.newMoveTo(false, 0); //first arg. : "CW or CCW?"
  }
}
Task music_player_stop_task(5000, 2, music_player_stop);

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
    // "P#SS-/-/-/" - P: P (play), SS: score #
    // "S-/-/-/-/-" - S: S (stop)
    //

    //parse command string.
    char first = msg.charAt(0);
    if (first == 'P') {
      //
      // "P#SS-/-/-/" - P: P (play), SS: score #
      //
      String str_score = msg.substring(2,4); // 23
      unsigned int score_cmd = str_score.toInt();
      //score #0 reserved as a RANDOM play.
      //  --> so, e.g. score #1 == note_steps[0][]
      //  --> so, e.g. score #2 == note_steps[1][]
      if (score_cmd > 0) {
        score_now = score_cmd - 1;
        if (score_now < SCORE_COUNT) {
          is_music_time = true;
          music_player_stepping_task.restart();
        }
      } else {
        // start a random player.
        is_music_time = true;
        // music_player_random_steps_task.restart();
      }
      //
    } else if (first == 'S') {
      //
      // "S-/-/-/-/-" - S: S (stop)
      //
      is_music_time = false;
      music_player_stop_task.restart();
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

  //relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  //i2c
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receiveEvent);

  //stepper pins
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  //tasks
  runner.init();
  runner.addTask(music_player_stepping_task);
  runner.addTask(music_player_stop_task);
}

void loop() {
  stepper.run();
  runner.execute();
}

// //DEBUG
// Serial.println("[SEQ]");
// Serial.print  ("  count: "); Serial.println(count);
// Serial.print  ("  step_seq_pos: "); Serial.println(step_seq_pos[count]);
// Serial.print  ("  step_pos_prev: "); Serial.println(step_pos_prev);
// Serial.print  ("  steps_to_move: "); Serial.println(steps_to_move);
// Serial.print  ("  moving_spd: "); Serial.print(moving_speed); Serial.println(" (steps/msec)");
// Serial.print  ("  max_spd: "); Serial.print(STEPS_PER_MILLISEC_MAX); Serial.println(" (steps/msec)");


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
//   // "P#SS-/-/-/" - P: P (play), SS: score #
//   // "S-/-/-/-/-" - S: S (stop)
//   //
//
