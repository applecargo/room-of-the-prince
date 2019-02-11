// i2c
#include <Wire.h>
#include "thunder/i2c_protocol.h"

// my tasks
extern Task rrrrr_task;
extern Task saying_greeting;
bool rrrrr_new = false;

//
extern Task msg_harmonica_task;

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
    case THUNDER_WORD_RRRRR:
      Serial.println("thunder: here we go! rrrrrrrrrrrr!");
      rrrrr_new = true;
      rrrrr_task.restartDelayed(2000);
      // // also, 'harmonica'!
      // msg_harmonica_task.restartDelayed(12000);
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
    sprintf(cmdstr, "F%04dA%04d", 500 + count * 50, 30);
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(cmdstr, CMD_LENGTH);
    Wire.endTransmission();
  }
  else {
    sprintf(cmdstr, "F%04dA%04d", 0, 0);
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(cmdstr, CMD_LENGTH);
    Wire.endTransmission();
  }
  mask = mask >> 1;
  count++;
}
Task reaction_task(10, 16, &reaction);

// saying hello
void greeting() {
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", ID_EVERYONE, THUNDER_WORD_HELLO); //"Hello? I do rrrrrrrrrrrr!"
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
}
Task saying_greeting(10000, TASK_FOREVER, &greeting);

// routine
extern Task routine_task;
void routine() {
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", ID_FLOAT, FLOAT_WORD_TURN_TURN);
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
  //
  routine_task.restartDelayed(random(1000*60*2, 1000*60*3));
}
Task routine_task(0, TASK_ONCE, &routine);

// make rrrrr noise
void rrrrr() {
  static int rrrrr_freq = 5;
  static int rrrrr_amp = 100;
  static int rrrrr_count = 0;
  if (rrrrr_new == true) {
    rrrrr_new = false;
    rrrrr_count = 0;
  }
  switch(random(5))
  // switch(rrrrr_count)
  {
  case 0:
    rrrrr_freq = 5;
    rrrrr_amp = 2000;
    break;
  case 1:
    rrrrr_freq = 8;
    rrrrr_amp = 1500;
    break;
  case 2:
    rrrrr_freq = 10;
    rrrrr_amp = 1500;
    break;
  case 3:
    rrrrr_freq = 15;
    rrrrr_amp = 1200;
    break;
  case 4:
    rrrrr_freq = 20;
    rrrrr_amp = 350;
    break;
  default:
    ;
  }
  //
  for (int idx = 0; idx < rrrrr_count; idx++) {
    Serial.print("r");
  }
  Serial.println("!");
  if (rrrrr_count == 24) {
    sprintf(cmdstr, "F%04dA%04d", 0, 1000);
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(cmdstr, CMD_LENGTH);
    Wire.endTransmission();
    //
    sprintf(msg_cstr, "[%06d:%03d] To everyone: thunder is gone!", ID_EVERYONE, THUNDER_WORD_LEFT);
    String str = String(msg_cstr);
    mesh.sendBroadcast(str);
    //
  } else {
    sprintf(cmdstr, "F%04dA%04d", rrrrr_freq, rrrrr_amp);
    Wire.beginTransmission(I2C_ADDR);
    Wire.write(cmdstr, CMD_LENGTH);
    Wire.endTransmission();
    //
    rrrrr_task.restartDelayed(random(100, 1000));
  }
  //
  rrrrr_count++;
}
Task rrrrr_task(0, TASK_ONCE, &rrrrr);

// msg_harmonica
void msg_harmonica() {
  // also, harmonica!
  sprintf(msg_cstr, "[%06d:%03d] To harmonica: you, too, amigo!", ID_HARMONICA, HARMONICA_WORD_PLAY_START);
  String str = String(msg_cstr);
  mesh.sendBroadcast(str);
}
Task msg_harmonica_task(0, TASK_ONCE, &msg_harmonica);

//setup_member
void setup_member() {
  //i2c master
  Wire.begin();

  //tasks
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(routine_task);
  routine_task.restart();

  runner.addTask(rrrrr_task);
  runner.addTask(reaction_task);

  //msg
  runner.addTask(msg_harmonica_task);
}
