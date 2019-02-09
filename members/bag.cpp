// i2c
#include <Wire.h>
#include "bag/i2c_protocol.h"

// servo
#include <Servo.h>
static Servo myservo;
//#define HANDLE_UP_TARGET 45 // MIN
#define HANDLE_UP_TARGET 50 // for 90 deg.
//#define HANDLE_DOWN_TARGET 151 // MAX
#define HANDLE_DOWN_TARGET 152 // for firm close

// my tasks
extern Task handle_up_task;
extern Task handle_down_task;
extern Task sing_task;
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
    case BAG_WORD_HANDLE_UP:
      Serial.println("bag: s-t-a-n-d up!");
      handle_up_task.restartDelayed(500);
      break;
    case BAG_WORD_HANDLE_DOWN:
      Serial.println("bag: seat down, now!");
      handle_down_task.restartDelayed(500);
      break;
    case BAG_WORD_SING:
      Serial.println("bag: s-i-n-g, now!");
      sing_task.restartDelayed(2000);
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
    ; // what to do?
  }
  else {
    ; // what to do?
  }
  mask = mask >> 1;
  count++;
}
Task reaction_task(10, 16, &reaction);

// saying hello
void greeting() {
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", ID_EVERYONE, BAG_WORD_HELLO); //"Sir! 9 of the 10!"
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
}
Task saying_greeting(10000, TASK_FOREVER, &greeting);

// handle up
void handle_up() {
  int angle = HANDLE_UP_TARGET;
  //
  Serial.print("at your service! :");
  Serial.print(angle);
  Serial.println(" deg.");
  //
  myservo.write(angle);
}
Task handle_up_task(0, TASK_ONCE, &handle_up);

// handle down
void handle_down() {
  int angle = HANDLE_DOWN_TARGET;
  //
  Serial.print("burrow! :");
  Serial.print(angle);
  Serial.println(" deg.");
  //
  myservo.write(angle);
}
Task handle_down_task(0, TASK_ONCE, &handle_down);

// sing!
void sing() {

  // "P#SSS@AAAA" - P: P (play), SSS: song #, A: amp. (x 1000)
  // "SXXXXXXXXX" - S: S (stop)

  sprintf(cmdstr, "P#%03d@%04d", 1, 1000); // play song #1, with amplitude == 1.0
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(cmdstr, CMD_LENGTH);
  Wire.endTransmission();
}
Task sing_task(0, TASK_ONCE, &sing);

//setup_member
void setup_member() {
  //i2c master
  Wire.begin();

  //servo
  myservo.attach(D6);

  //tasks
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(handle_up_task);
  runner.addTask(handle_down_task);
  runner.addTask(sing_task);
  runner.addTask(reaction_task);

  //
  handle_down_task.restartDelayed(500);
}
