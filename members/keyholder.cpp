// servo
#include <Servo.h>
static Servo myservo1;
static Servo myservo2;
static Servo myservo3;
#define HOLDING_ANGLE  40
#define HOLDING_ANGLE2 40
#define FALLING_ANGLE  22
#define FALLING_ANGLE2 56
#define FALLING_TIME   1000
#define HOLDING_TIME   200

// my tasks
extern Task holdingkey1_task;
extern Task fallingkey1_task;
extern Task holdingkey2_task;
extern Task fallingkey2_task;
extern Task holdingkey3_task;
extern Task fallingkey3_task;
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
    case KEYHOLDER_WORD_FALLING_KEYS:
      Serial.println("keyholder: keys!~ leave away~");
      fallingkey1_task.restart();
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

  }
  else {

  }
  mask = mask >> 1;
  count++;
}
Task reaction_task(10, 16, &reaction);

// saying hello
void greeting() {
  static String greeting = "red key, yellow key, blue key";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  mesh.sendBroadcast(greeting_r);
}
Task saying_greeting(2000, TASK_FOREVER, &greeting);

void fallingkey1() {
  static bool first = true;
  static int lasttime = millis();
  if (first || millis() - lasttime > 10000) { // block re-trigger for 10 sec.
    int angle1 = FALLING_ANGLE;
    myservo1.write(angle1);
    //
    Serial.print("red key fall down now:");
    Serial.println(angle1);
    //
    holdingkey1_task.restartDelayed(FALLING_TIME);
    //
    lasttime = millis();
    first = false;
  }
}
Task fallingkey1_task(0, TASK_ONCE, &fallingkey1);

void holdingkey1() {
  int angle1 = HOLDING_ANGLE;
  myservo1.write(angle1);
  //
  Serial.print("wait for the keys");
  Serial.println(angle1);
  fallingkey2_task.restartDelayed(HOLDING_TIME);
}
Task holdingkey1_task(0, TASK_ONCE, &holdingkey1);

void fallingkey2() {
  static bool first = true;
  static int lasttime = millis();
  if (first || millis() - lasttime > 10000) { // block re-trigger for 10 sec.
    int angle2 = FALLING_ANGLE2;
    myservo2.write(angle2);
    //
    Serial.print("red key fall down now:");
    Serial.println(angle2);
    //
    holdingkey2_task.restartDelayed(FALLING_TIME);
    //
    lasttime = millis();
    first = false;
  }
}
Task fallingkey2_task(0, TASK_ONCE, &fallingkey2);

void holdingkey2() {
  int angle2 = HOLDING_ANGLE2;
  myservo2.write(angle2);
  //
  Serial.print("wait for the keys");
  Serial.println(angle2);
  fallingkey3_task.restartDelayed(HOLDING_TIME);
}
Task holdingkey2_task(0, TASK_ONCE, &holdingkey2);

void fallingkey3() {
  static bool first = true;
  static int lasttime = millis();
  if (first || millis() - lasttime > 10000) { // block re-trigger for 10 sec.
    int angle3 = FALLING_ANGLE;
    myservo3.write(angle3);
    //
    Serial.print("red key fall down now:");
    Serial.println(angle3);
    //
    holdingkey3_task.restartDelayed(FALLING_TIME);
    //
    lasttime = millis();
    first = false;
  }
}
Task fallingkey3_task(0, TASK_ONCE, &fallingkey3);

void holdingkey3() {
  int angle3 = HOLDING_ANGLE;
  myservo3.write(angle3);
  //
  Serial.print("wait for the keys");
  Serial.println(angle3);
}
Task holdingkey3_task(0, TASK_ONCE, &holdingkey3);
//setup_member
void setup_member() {

  //servo
  myservo1.attach(D6);
  myservo2.attach(D7);
  myservo3.attach(D8);



  //tasks
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(holdingkey1_task);
  runner.addTask(fallingkey1_task);
  runner.addTask(holdingkey2_task);
  runner.addTask(fallingkey2_task);
  runner.addTask(holdingkey3_task);
  runner.addTask(fallingkey3_task);
  runner.addTask(reaction_task);

  //
  holdingkey1_task.restart();
  holdingkey2_task.restart();
  holdingkey3_task.restart();
}
