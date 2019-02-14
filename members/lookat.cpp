// servo
#define SERVO_PIN D6
#include <Servo.h>
static Servo myservo;
extern Task servo_release_task;

// my tasks
extern Task lookat_task;
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
    case LOOKAT_WORD_LOOK_AROUND:
      Serial.println("lookat: me to looking around?? well, but where?");
      lookat_task.restart();
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
    tone(D7, 800 + count * 50);
  }
  else {
    noTone(D7);
  }
  if (reaction_task.isLastIteration()) {
    noTone(D7);
  }
  mask = mask >> 1;
  count++;
}
Task reaction_task(10, 17, &reaction);

// saying hello
void greeting() {
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", memberList[random(NUM_OF_MEMBERS)], LOOKAT_WORD_HELLO); //"Me? I do look at you!"
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
}
Task saying_greeting(10000, TASK_FOREVER, &greeting);

// routine
extern Task routine_task;
void routine() {
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", ID_ROCKING, ROCKING_WORD_ROCK_ROCK_ROCK);
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
  //
  routine_task.restartDelayed(random(1000*60*5, 1000*60*7));
}
Task routine_task(0, TASK_ONCE, &routine);

// looking around once.
void lookat() {
  int angle = random(0, 180);
  //
  Serial.print("i will look at now @ ");
  Serial.print(angle);
  Serial.println(" deg.");
  //
  myservo.attach(D6);
  myservo.write(angle);
  servo_release_task.restartDelayed(100);
}
Task lookat_task(0, TASK_ONCE, &lookat);

// servo release
void servo_release() {
  myservo.detach();
}
Task servo_release_task(0, TASK_ONCE, &servo_release);

//setup_member
void setup_member() {

  //servo
  // myservo.attach(D6);

  //tone
  pinMode(D7, OUTPUT);

  //tasks
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(routine_task);
  routine_task.enable();

  runner.addTask(routine_task);
  routine_task.enable();
  //
  runner.addTask(lookat_task);
  //
  runner.addTask(servo_release_task);
  runner.addTask(reaction_task);
}
