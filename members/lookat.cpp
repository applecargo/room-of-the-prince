#include <Servo.h>

//
extern Task lookat_task;
extern Task lookat_msg_task;

//
static Servo myservo;

//
static int content = 0;

// room protocol
void gotMessageCallback(uint32_t from, String & msg) { // REQUIRED
  // Serial.println("RX:" + msg);
  Serial.println(msg);

  // is it for me?
  // Serial.print("receipent:");
  int receipent = msg.substring(1, 7).toInt();
  // Serial.println(receipent);

  if (receipent == ID_LOOK_AT) {

    // what it says?
    // Serial.print("content:");
    content = msg.substring(8, 12).toInt();
    // Serial.println(content);

    switch (content)
    {
    case LOOKAT_WORD_LOOK_AROUND:
      Serial.println("lookat: me to looking around?? well, but where?");
      lookat_msg_task.restart();
      lookat_task.restart();
      break;
    default:
      ;
    }

  }
}
void gotChangedConnectionCallback() { // REQUIRED
  ;
}

// task #1 : my stuff
// let's blindly say hello to everyone.
void greeting() {
  static String greeting = "Hello? I will look at you!";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  // Serial.println("TX:" + greeting_r);
  mesh.sendBroadcast(greeting_r);
}
Task saying_greeting(1000, TASK_FOREVER, &greeting);

// look_at. express received msg.
static int mask = 0x8000; // 16bit mask --> iteration of 16 times.
static int tgl_cnt = 0; // 16bit mask --> iteration of 16 times.

// by relay toggling

// void relayToggler() {
//   if (lookat_msg_task.isFirstIteration()) {
//     mask = 0x8000;
//   }
//   // Serial.print("bit-seq: ");
//   // Serial.println((content & mask) == 0);
//   if ((content & mask) == 0) {
//     //relay ON.
//     digitalWrite(D7, HIGH);
//   }
//   else {
//     //relay OFF.
//     digitalWrite(D7, LOW);
//   }
//   //
//   mask = mask >> 1;
// }
// //the task
// Task lookat_msg_task(10, 16, &relayToggler);

// by buzzer messaging

void buzzerMessage() {
  if (lookat_msg_task.isFirstIteration()) {
    mask = 0x8000;
    tgl_cnt = 0;
  }
  // Serial.print("bit-seq: ");
  // Serial.println((content & mask) == 0);
  if ((content & mask) == 0) {
    //buzzer ON.
    tone(D7, 1500 + tgl_cnt * 50);
    // tone(D7, 300 + (int)(tgl_cnt * random(50)));
    // tone(D7, 3000 + (int)random(1000));
  }
  else {
    //relay OFF.
    noTone(D7);
  }
  //
  mask = mask >> 1;
  tgl_cnt++;
}
//the task
Task lookat_msg_task(10, 16, &buzzerMessage);

// looking around once.
void lookat() {
  int angle = random(0, 180);
  //
  Serial.print("i will look at now @ ");
  Serial.println(angle);
  //
  myservo.write(angle);
}
//the task
Task lookat_task(0, TASK_ONCE, &lookat);

//setup_member
void setup_member() {
  //
  myservo.attach(D6);
  //
  pinMode(D7, OUTPUT);
  // digitalWrite(D7, HIGH);
  //
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  //
  runner.addTask(lookat_task);
  runner.addTask(lookat_msg_task);
}
