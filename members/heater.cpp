//tasks
extern Task fastturn_task;
extern Task slowturn_task;
extern Task moreslowturn_task;
extern Task rest_task;

#define FASTTRUN 500
#define SLOWTURN 200
#define STOP 0
#define REST_TIME 20

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
    case HEATER_WORD_NOISY_NOISY:
      Serial.println("heater: welcome to noise world");
      fastturn_task.restartDelayed(100);
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
  sprintf(msg_cstr, "[%06d:%03d]", memberList[random(NUM_OF_MEMBERS)], HEATER_WORD_HELLO); //"(rrrrrrrrrrr)"
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
}
Task saying_greeting(10000, TASK_FOREVER, &greeting);

// routine
extern Task routine_task;
void routine() {
  static String msg = "";
  static bool handle_updown = false;
  handle_updown = !handle_updown;
  sprintf(msg_cstr, "[%06d:%03d]", ID_BAG, (handle_updown ? BAG_WORD_HANDLE_UP : BAG_WORD_HANDLE_DOWN));
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
  //
  routine_task.restartDelayed(random(1000*60*4, 1000*60*6));
}
Task routine_task(0, TASK_ONCE, &routine);

void fastturn() {
  analogWrite(D6,400);
  slowturn_task.restartDelayed(300);
}
Task fastturn_task(0, TASK_ONCE, &fastturn);

// handle down
void slowturn() {
  analogWrite(D6,200);
  moreslowturn_task.restartDelayed(10000);
}
Task slowturn_task(0, TASK_ONCE, &slowturn);

// handle down
void moreslowturn() {
  analogWrite(D6,150);
  rest_task.restartDelayed(10000);
}
Task moreslowturn_task(0, TASK_ONCE, &moreslowturn);

void rest() {
  analogWrite(D6,0);
}
Task rest_task(0, TASK_ONCE, &rest);

void setup_member() {
  //pwm
  pinMode(D6, OUTPUT);

  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(routine_task);
  routine_task.restart();

  runner.addTask(fastturn_task);
  runner.addTask(slowturn_task);
  runner.addTask(moreslowturn_task);
  runner.addTask(reaction_task);
  runner.addTask(rest_task);

  rest_task.restartDelayed(500);
}
