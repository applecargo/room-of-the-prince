// servo
#include <Servo.h>
static Servo myservo1;
static Servo myservo2;
static int angle = 0;

#define REST_TIME   25

// my tasks
extern Task base_task;
extern Task movingup_task;
extern Task movingdown_task;
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
    case MIRROR_WORD_IWILLSHOWYOU:
      Serial.println("mirror: I will show you");
      movingup_task.restart();
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
  static String greeting = "show you, see you, where are you?";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  mesh.sendBroadcast(greeting_r);
}
Task saying_greeting(2000, TASK_FOREVER, &greeting);

//
void movingup() {
  angle++;
  myservo1.write(angle);
  myservo2.write(angle);
  Serial.print("moving, moving");
  Serial.println(angle);
  if (angle < 180) {
    movingup_task.restartDelayed(REST_TIME);
  } else{
    movingdown_task.restartDelayed(REST_TIME);
  }
}
Task movingup_task(0, TASK_ONCE, &movingup);

void movingdown() {
  angle--;
  myservo1.write(angle);
  myservo2.write(angle);
  Serial.print("move move");
  Serial.println(angle);
  if (angle > 0) {
    movingdown_task.restartDelayed(REST_TIME);
  } else{
    base_task.restartDelayed(REST_TIME);
  }
}
Task movingdown_task(0, TASK_ONCE, &movingdown);

void base() {
  angle = 0;
  myservo1.write(angle);
  myservo2.write(angle);
}
Task base_task(0, TASK_ONCE, &base);

//setup_member
void setup_member() {

  //servo
  myservo1.attach(D6);
  myservo2.attach(D7);

  //tasks
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(movingup_task);
  runner.addTask(movingdown_task);
  runner.addTask(base_task);
  runner.addTask(reaction_task);

  //
  base_task.restart();
}
