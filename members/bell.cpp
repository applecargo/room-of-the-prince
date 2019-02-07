// i2c
#include <Wire.h>
#include "fur/i2c_protocol.h"

// servo
#include <Servo.h>
static Servo myservo;



extern Task standby_task;
extern Task belling_task;
extern Task rest_task;

#define STANDBY_ANGLE 60
#define BELLING_ANGLE 87
#define REST_ANGLE 70

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

    case BELL_WORD_RING_RING_RING:
      Serial.println("bell: ring ring ");
      belling_task.restartDelayed(100);
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
  static String greeting = "signal out~ take my signal~~ to every";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  mesh.sendBroadcast(greeting_r);
}
Task saying_greeting(1000, TASK_FOREVER, &greeting);

void belling() {
  int angle = BELLING_ANGLE;
  //
  Serial.print("bell bell bell :");
  Serial.print(angle);
  Serial.println(" deg.");
  //
  myservo.write(angle);
  standby_task.restartDelayed(100);
}
Task belling_task(0, TASK_ONCE, &belling);

// handle down
void standby() {
  int angle = STANDBY_ANGLE;
  //
  Serial.print("no bell :");
  Serial.print(angle);
  Serial.println(" deg.");
  //
  myservo.write(angle);
  rest_task.restartDelayed(100);
}
Task standby_task(0, TASK_ONCE, &standby);

void rest() {
  int angle = REST_ANGLE;
  //
  Serial.print("no bell :");
  Serial.print(angle);
  Serial.println(" deg.");
  //
  myservo.write(angle);
}
Task rest_task(0, TASK_ONCE, &rest);


void setup_member() {
  //i2c master
  Wire.begin();
  myservo.attach(D6);

  runner.addTask(saying_greeting);
  saying_greeting.enable();

  runner.addTask(belling_task);
  runner.addTask(standby_task);
  runner.addTask(reaction_task);
  runner.addTask(rest_task);

  rest_task.restartDelayed(500);
}
