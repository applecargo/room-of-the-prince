// my tasks
extern Task motion_task;
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
    // case MOTION_WORD_MOTION_START:
    //   break;
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
    digitalWrite(D7, HIGH);
  }
  else {
    digitalWrite(D7, LOW);
  }
  mask = mask >> 1;
  count++;
}
Task reaction_task(10, 16, &reaction);

// saying hello
void greeting() {
  static String greeting = "Hello? My name is.. Mother Detroit.";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  mesh.sendBroadcast(greeting_r);
}
Task saying_greeting(1000, TASK_FOREVER, &greeting);

// motion detection
void motion() {
  static bool motion_prev = false;
  static String msg = "";
  bool motion = (digitalRead(D1) == HIGH);
  if (motion_prev != motion) {
    if (motion == true) {
      // Serial.println("motion start.");
      sprintf(msg_cstr, "[%06d:%03d] To everyone: Hey, something is moving now!", ID_EVERYONE, MOTION_WORD_MOTION_START);
      msg = String(msg_cstr);
      mesh.sendBroadcast(msg);
      //
      sprintf(msg_cstr, "[%06d:%03d] To lookat: look around now!", ID_LOOK_AT, LOOKAT_WORD_LOOK_AROUND);
      msg = String(msg_cstr);
      mesh.sendBroadcast(msg);
      //
      message = MOTION_WORD_MOTION_START;
      reaction_task.restart();
    } else {
      // Serial.println("motion stop.");
      sprintf(msg_cstr, "[%06d:%03d] To everyone: Uhm, my world is tran·quilo.", ID_EVERYONE, MOTION_WORD_MOTION_START);
      msg = String(msg_cstr);
      mesh.sendBroadcast(msg);
      //
      message = MOTION_WORD_MOTION_END;
      reaction_task.restart();
    }
  }
  motion_prev = motion;
}
Task motion_task(20, TASK_FOREVER, &motion);

//setup_member
void setup_member() {
  //
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  //
  runner.addTask(motion_task);
  motion_task.enable();
  //
  runner.addTask(reaction_task);
}
