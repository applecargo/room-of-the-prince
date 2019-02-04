#include <Arduino.h>

//relay
#define RELAY_PIN 5

//
#include <AccelStepper.h>
#define RPM_MAX               (18.0)
#define STEPS_PER_REV         (4096.0)
//
// speed (rpm) * steps-per-revolution == speed (steps per minute)
//  --> speed (steps per minute) / 60 == speed (steps per second)
//  --> speed (steps per second) * 60 / steps-per-revolution == speed (rpm)
#define STEPS_PER_SEC_TO_RPM (60.0 / STEPS_PER_REV)
#define RPM_TO_STEPS_PER_SEC (STEPS_PER_REV / 60.0)
//
#define STEPS_PER_SEC_MAX    (RPM_MAX * RPM_TO_STEPS_PER_SEC)

//
AccelStepper stepper(AccelStepper::HALF4WIRE, 8, 9, 10, 11);

//score list
static int score_now = 0;
//score #0 reserved as a RANDOM play.
#define SCORE_NOTE_MAX 8
#define SCORE_COUNT 2
//score #1
static int note_idx = 0;
static int notes[SCORE_COUNT][SCORE_NOTE_MAX][2] = { //unit: (steps, millisec)

  //score #1
  {
    {  1000,  2000},
    {  3000,  2000},
    {  1000,  3000},
    { 10000,   100},
    {  3000,  2000},
    { 10000,  2500},
    {  1000,  1000},
    {   100,   400}
  },

  // score #2
  {
    {  1000,  2000},
    {  3000,  2000},
    {  1000,  3000},
    { 10000,   100},
    {  3000,  2000},
    { 10000,  2500},
    {  1000,  1000},
    {   100,   400}
  }
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
  if (stepper.distanceToGo() == 0 && is_music_time == true) {
    //
    digitalWrite(RELAY_PIN, HIGH); // blow start! (and continue.)
    //
    float cur_step = stepper.currentPosition();
    float target_step = notes[score_now][note_idx][0];
    float dur = notes[score_now][note_idx][1];
    float steps = target_step - cur_step;
    float velocity = steps / dur * 1000; // unit conv.: (steps/msec) --> (steps/sec)
    float speed = fabs(velocity);
    //
    if (speed > STEPS_PER_SEC_MAX) {
      Serial.println("oh.. isn't it TOO FAST??");
    } else {
      Serial.println("okay. i go now.");
    }

    //DEBUG
    Serial.print(" --> dur : "); Serial.println(dur);
    Serial.print(" --> cur_step : "); Serial.println(cur_step);
    Serial.print(" --> target_step : "); Serial.println(target_step);
    Serial.print(" --> steps : "); Serial.println(steps);
    Serial.print(" --> velocity(steps/sec) : "); Serial.println(velocity);
    Serial.print(" --> speed(steps/sec) : "); Serial.println(speed);
    Serial.print(" --> speed(rpm) : "); Serial.println(speed * STEPS_PER_SEC_TO_RPM);
    //
    stepper.moveTo(target_step);
    stepper.setSpeed(velocity);
    //NOTE: 'setSpeed' should come LATER than 'moveTo'!
    //  --> 'moveTo' re-calculate the velocity.
    //  --> so we need to re-override it.
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
    // Serial.println("stepper BUSY! will wait a bit.");
  }
}
Task music_player_stepping_task(0, TASK_ONCE, music_player_stepping);

//
void music_player_stop() {
  if (music_player_stop_task.isFirstIteration()) {
    //block 'music_player_stepping_task'
    is_music_time = false;
    // stop for sometime! (while blowing stops..)
    stepper.stop();
    // blow stop!
    digitalWrite(RELAY_PIN, LOW);
  } else {
    //okay, blower is over, now. then, go home position.
    stepper.moveTo(0);
    stepper.setSpeed(20 * RPM_TO_STEPS_PER_SEC); // 20 rpm
    //NOTE: 'setSpeed' should come LATER than 'moveTo'!
    //  --> 'moveTo' re-calculate the velocity.
    //  --> so we need to re-override it.
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
      //  --> so, e.g. score #1 == notes[0][][]
      //  --> so, e.g. score #2 == notes[1][][]
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

  //stepper
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  /// "
  /// The fastest motor speed that can be reliably supported is about 4000 steps per
  /// second at a clock frequency of 16 MHz on Arduino such as Uno etc.
  /// "
  stepper.setMaxSpeed(1000); //steps per second (trade-off between speed vs. torque)

  //tasks
  runner.init();
  runner.addTask(music_player_stepping_task);
  runner.addTask(music_player_stop_task);
}

void loop() {
  //task
  runner.execute();

  //stepper
  if (stepper.distanceToGo() != 0) {
    stepper.runSpeed();
  }
}
