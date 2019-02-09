// my tasks
extern Task door_task;
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
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", ID_EVERYONE, DOOR_WORD_HELLO); //"Kein Problem. Die Tür ist jetzt geöffnet!"
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
}
Task saying_greeting(10000, TASK_FOREVER, &greeting);

// door detection
void door() {
  static bool door_stat_prev = false;
  static String msg = "";
  bool door_stat = (6762/analogRead(A0) > 20);
  if (door_stat_prev != door_stat) {
    if (door_stat == true) {
      Serial.println("door opened.");
      sprintf(msg_cstr, "[%06d:%03d] To everyone: Ich bin geöffnet, etwas geht an mir vorbei!", ID_EVERYONE, DOOR_WORD_PASSING_BY);
      msg = String(msg_cstr);
      mesh.sendBroadcast(msg);
      //
      message = DOOR_WORD_PASSING_BY;
      reaction_task.restart();
    } else {
      Serial.println("door closed.");
      sprintf(msg_cstr, "[%06d:%03d] To everyone: Ähm, keine Passagiere.", ID_EVERYONE, DOOR_WORD_NO_PASSENGER);
      msg = String(msg_cstr);
      mesh.sendBroadcast(msg);
      //
      message = DOOR_WORD_NO_PASSENGER;
      reaction_task.restart();
    }
  }
  door_stat_prev = door_stat;
}
Task door_task(20, TASK_FOREVER, &door);

//setup_member
void setup_member() {
  //
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  //
  runner.addTask(door_task);
  door_task.enable();
  //
  runner.addTask(reaction_task);
}
