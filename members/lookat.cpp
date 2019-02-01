// servo
#include <Servo.h>
static Servo myservo;

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
  // Serial.println(msg);
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
  static String greeting = "Me? I do look at you!";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  mesh.sendBroadcast(greeting_r);
}
Task saying_greeting(1000, TASK_FOREVER, &greeting);

// looking around once.
void lookat() {
  int angle = random(0, 180);
  //
  Serial.print("i will look at now @ ");
  Serial.print(angle);
  Serial.println(" deg.");
  //
  myservo.write(angle);
}
Task lookat_task(0, TASK_ONCE, &lookat);

//setup_member
void setup_member() {

  //servo
  myservo.attach(D6);

  //tone
  pinMode(D7, OUTPUT);

  //tasks
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(lookat_task);
  runner.addTask(reaction_task);
}
