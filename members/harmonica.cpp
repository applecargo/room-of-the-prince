// i2c
#include <Wire.h>
#include "harmonica/metrics.h"
#include "harmonica/i2c_protocol.h"

// my tasks
extern Task step_writer_task;
extern Task saying_greeting;

// room protocol
static int message = 0;
static char msg_cstr[MSG_LENGTH_MAX] = {0, };
extern Task reaction_task;
void gotChangedConnectionCallback() { // REQUIRED
}
void gotMessageCallback(uint32_t from, String & msg) { // REQUIRED
  Serial.println(msg);
  // is it for me?
  int receipent = msg.substring(1, 7).toInt();
  if (receipent == IDENTITY) {
    // what it says?
    message = msg.substring(8, 12).toInt();
    // i ve heard. reaction.
    reaction_task.restart();
    // so, what to do, then?
    switch (message)
    {
    case HARMONICA_WORD_PLAY_START:
      step_writer_task.restart();
      break;
    default:
      ;
    }
  }
}

// some reaction for received msg.
void reaction() {
  static int mask = 0x8000;
  static int count = 0;
  if (reaction_task.isFirstIteration()) {
    mask = 0x8000;
    count = 0;
  }
  if ((message & mask) == 0) {
    //what to do?
  }
  else {
    //what to do?
  }
  mask = mask >> 1;
  count++;
}
Task reaction_task(10, 16, &reaction);

// saying hello
void greeting() {
  static String greeting = "MUSIC is your life.";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  mesh.sendBroadcast(greeting_r);
}
Task saying_greeting(1000, TASK_FOREVER, &greeting);

// step writer
// static int step_seq_pos[64] = {1, };   // unit: steps
// static int step_seq_dur[64] = {100, }; // unit: msec
static int step_seq_pos[64] = { \
  100, 200, 300, 200, 100, 300, 1000, 1, };   // unit: steps
static int step_seq_dur[64] = { \
  100, 100, 100, 100, 100, 100, 100, 100, \
  100, 100, 100, 100, 100, 100, 100, 100, \
  100, 100, 100, 100, 100, 100, 100, 100, \
  100, 100, 100, 100, 100, 100, 100, 100, \
  100, 100, 100, 100, 100, 100, 100, 100, \
  100, 100, 100, 100, 100, 100, 100, 100, \
  100, 100, 100, 100, 100, 100, 100, 100, \
  100, 100, 100, 100, 100, 100, 100, 100}; // unit: msec
void step_writer() {
  static int count = 0;
  static int step_pos_prev = 0;
  static char cstr[256] = {0, };
  if (count < 64) {
    //
    // sanity check (speed)
    //
    float steps_to_move = step_seq_pos[count] - step_pos_prev;
    float moving_speed = steps_to_move / step_seq_dur[count]; // unit: (steps/msec)
    if (steps_to_move != 0) {
      if (fabs(moving_speed) > STEPS_PER_MILLISEC_MAX) {
        //oh.. that might be TOO fast, isn't it??
        Serial.println("oh.. isn't it TOO FAST??");
      } else {
        //my next move will be this fast.. and i can do that.
        Serial.println("okay. i go now.");
      }
    }

    //DEBUG
    Serial.println("[SEQ]");
    Serial.print  ("  count: "); Serial.println(count);
    Serial.print  ("  step_seq_pos: "); Serial.println(step_seq_pos[count]);
    Serial.print  ("  step_pos_prev: "); Serial.println(step_pos_prev);
    Serial.print  ("  steps_to_move: "); Serial.println(steps_to_move);
    Serial.print  ("  moving_spd: "); Serial.print(moving_speed); Serial.println(" (steps/msec)");
    Serial.print  ("  max_spd: "); Serial.print(STEPS_PER_MILLISEC_MAX); Serial.println(" (steps/msec)");

    //
    // "PppppDdddd"
    //   'P' - target position (steps)
    //   'D' - movement duration (msec)
    //

    //
    sprintf(cmdstr, "P%04dS%04d", step_seq_pos[count], step_seq_dur[count]);
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(cmdstr);
    Wire.endTransmission();
    //
    step_writer_task.restartDelayed(step_seq_dur[count]); //msec
    //
    step_pos_prev = step_seq_pos[count];
  }
  else {
    count = 0;
  }
  //
  count++;
}
Task step_writer_task(0, TASK_ONCE, &step_writer);

//setup_member
void setup_member() {
  //i2c master
  Wire.begin();

  //tasks
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(step_writer_task);
  runner.addTask(reaction_task);
}
