// i2c
#include <Wire.h>
#include "distance/i2c_protocol.h"

// my tasks
extern Task distance_task;
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
    if (reaction_task.getRunCounter() == 0)
      reaction_task.restart();
    // so, what to do, then?
    switch (message)
    {
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
  if (reaction_task.isLastIteration()) {
    //
  }
  mask = mask >> 1;
  count++;
}
Task reaction_task(10, 17, &reaction);

// saying hello
void greeting() {
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", memberList[random(NUM_OF_MEMBERS)], DISTANCE_WORD_HELLO); //"You know what? The earth is ROUND."
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
}
Task saying_greeting(10000, TASK_FOREVER, &greeting);

// routine
extern Task routine_task;
void routine() {
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", ID_HARMONICA, HARMONICA_WORD_PLAY_START);
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
  //
  routine_task.restartDelayed(random(1000*60*6, 1000*60*8));
}
Task routine_task(0, TASK_ONCE, &routine);

// distance
void distance_reader() {
  static bool presence = false;
  static bool presence_prev = false;

  //fetch cmdstr
  Wire.requestFrom(I2C_ADDR, CMD_LENGTH); //request cmdstr.
  if (CMD_LENGTH == Wire.readBytes(cmdstr, CMD_LENGTH)) { // receive cmdstr.
    //convert to String
    String msg = String(cmdstr);

    //
    // "D000XXXXXX" - D: distance(cm)
    //

    //
    Serial.println(msg);

    String str = msg.substring(1, 4); // 123
    int dist_cm = str.toInt();

    //
    presence = (dist_cm > 80 && dist_cm < 300);

    if (presence != presence_prev && presence == false) {
      // someone passed under me.
      sprintf(msg_cstr, "[%06d:%03d] To everyone: Now, prince is in ACTION!", ID_EVERYONE, DISTANCE_WORD_PRINCE_ACTIVE);
      msg = String(msg_cstr);
      mesh.sendBroadcast(msg);
      //
      Serial.println(msg_cstr);
    }

    //
    presence_prev = presence;
  }
}
Task distance_task(300, TASK_FOREVER, &distance_reader);

//setup_member
void setup_member() {
  //i2c master
  Wire.begin();

  //tasks
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(routine_task);
  routine_task.restart();

  runner.addTask(distance_task);
  distance_task.enable();
  runner.addTask(reaction_task);
}
