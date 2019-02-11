// i2c
#include <Wire.h>
#include "harmonica/i2c_protocol.h"

// my tasks
extern Task play_music_task;
extern Task stop_music_task;
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
      play_music_task.restart();
      break;
    case HARMONICA_WORD_PLAY_STOP:
      stop_music_task.restart();
      break;
    default:
      ;
    }
  }
  //eavesdropping 'thunder' message
  if (receipent == ID_THUNDER) {
    // what it says?
    message = msg.substring(8, 12).toInt();
    // i ve heard. reaction.
    reaction_task.restart();
    // so, what to do, then?
    switch (message)
    {
    case THUNDER_WORD_RRRRR:
      play_music_task.restartDelayed(10000);
      Serial.println("The prince is alive!");
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
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", ID_EVERYONE, HARMONICA_WORD_HELLO); //"MUSIC is your life."
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
}
Task saying_greeting(10000, TASK_FOREVER, &greeting);

// routine
extern Task routine_task;
void routine() {
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", ID_HEATER, HEATER_WORD_NOISY_NOISY);
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
  //
  routine_task.restartDelayed(random(1000*60*5, 1000*60*8));
}
Task routine_task(0, TASK_ONCE, &routine);

// play music
void play_music() {

  //
  // "P#SS-/-/-/" - P: P (play), SS: score #
  //

  //
  sprintf(cmdstr, "P#%02d-/-/-/", 0); // score #0 is SPECIAL --> a random play.
  // sprintf(cmdstr, "P#%02d-/-/-/", 1);
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(cmdstr);
  Wire.endTransmission();
}
Task play_music_task(0, TASK_ONCE, &play_music);

// stop music
void stop_music() {

  //
  // "S-/-/-/-/-" - S: S (stop)
  //

  //
  sprintf(cmdstr, "S-/-/-/-/-");
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(cmdstr);
  Wire.endTransmission();
}
Task stop_music_task(0, TASK_ONCE, &stop_music);

//setup_member
void setup_member() {
  //i2c master
  Wire.begin();

  //tasks
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(routine_task);
  routine_task.restart();

  runner.addTask(play_music_task);
  runner.addTask(stop_music_task);
  runner.addTask(reaction_task);
}
