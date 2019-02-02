// servo
#include <Servo.h>
static Servo myservo;
#define ROCK_TARGET 170
#define REST_TARGET 20
#define ROCK1_TIME   500
#define ROCK2_TIME   800

// my tasks
extern Task rocking_task;
extern Task rocking_more_task;
extern Task resting_task;
extern Task timing_task;
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
    case ROCKING_WORD_ROCK_ROCK_ROCK:
      Serial.println("rocking: rock, rock, rock");
      rocking_task.restart();
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
    tone(D7, 1500 + count * 50);
  }
  else {
    noTone(D7);
  }
  mask = mask >> 1;
  count++;
}
Task reaction_task(10, 16, &reaction);

// saying hello
void greeting() {
  static String greeting = "ROCK is the spirit!";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  mesh.sendBroadcast(greeting_r);
}
Task saying_greeting(1000, TASK_FOREVER, &greeting);

void rocking() {
  static bool first = true;
  static int lasttime = millis();
  if (first || millis() - lasttime > 10000) { // block re-trigger for 10 sec.
    int angle = ROCK_TARGET;
    myservo.write(angle);
    //
    Serial.print("rocking now:");
    Serial.println(angle);
    //
    timing_task.restartDelayed(ROCK1_TIME);
    //
    lasttime = millis();
    first = false;
  }
}
Task rocking_task(0, TASK_ONCE, &rocking);

void timing() {
  int angle = REST_TARGET;
  myservo.write(angle);
  //
  Serial.print("timing now:");
  Serial.println(angle);
  //
  rocking_more_task.restartDelayed(ROCK2_TIME);
}
Task timing_task(0, TASK_ONCE, &timing);

void rocking_more() {
  int angle = ROCK_TARGET;
  myservo.write(angle);
  //
  Serial.print("rocking more!:");
  Serial.println(angle);
  //
  resting_task.restartDelayed(ROCK1_TIME);
}
Task rocking_more_task(0, TASK_ONCE, &rocking_more);

void resting() {
  int angle = REST_TARGET;
  myservo.write(angle);
  //
  Serial.print("resting now:");
  Serial.println(angle);
}
Task resting_task(0, TASK_ONCE, &resting);

//setup_member
void setup_member() {

  //servo
  myservo.attach(D6);

  //tone
  pinMode(D7, OUTPUT);

  //tasks
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(rocking_task);
  runner.addTask(rocking_more_task);
  runner.addTask(timing_task);
  runner.addTask(resting_task);
  runner.addTask(reaction_task);

  //
  resting_task.restart();
}
