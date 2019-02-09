// my tasks
extern Task tune_task;
extern Task saying_greeting;

//tunes
#define TUNE_EXACT 0
#define TUNE_HIGH  1
#define TUNE_LOW   2

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
    // digitalWrite(D7, HIGH);
  }
  else {
    // digitalWrite(D7, LOW);
  }
  mask = mask >> 1;
  count++;
}
Task reaction_task(10, 16, &reaction);

// saying hello
void greeting() {
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", ID_EVERYONE, TUNER_WORD_HELLO); //"I am Tuba Carlos. Don't worry to much about tones. Just let it go. Chill-out~"
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
}
Task saying_greeting(10000, TASK_FOREVER, &greeting);

// tune detection
void tune() {
  static int level = TUNE_EXACT;
  static int level_prev = TUNE_EXACT;
  static String msg = "";
  //
  level = TUNE_EXACT;
  if (digitalRead(D5) == HIGH) level = TUNE_HIGH;
  if (digitalRead(D6) == HIGH) level = TUNE_LOW;
  //
  if (level_prev != level) {
    if (level == TUNE_EXACT) {
      Serial.println("the tune is perfect!");
      sprintf(msg_cstr, "[%06d:%03d] To everyone: one strike!", ID_EVERYONE, TUNER_WORD_THIS_IS_CORRECT);
      msg = String(msg_cstr);
      mesh.sendBroadcast(msg);
      //
      message = TUNER_WORD_THIS_IS_CORRECT;
      reaction_task.restart();
    } else if (level == TUNE_LOW) {
      Serial.println("the tune is a bit low_____...");
      sprintf(msg_cstr, "[%06d:%03d] To everyone: one low ball!", ID_EVERYONE, TUNER_WORD_THIS_IS_LOW);
      msg = String(msg_cstr);
      mesh.sendBroadcast(msg);
      //
      message = TUNER_WORD_THIS_IS_LOW;
      reaction_task.restart();
    } else if (level == TUNE_HIGH) {
      Serial.println("the tune is a bit high!!...");
      sprintf(msg_cstr, "[%06d:%03d] To everyone: one high ball!", ID_EVERYONE, TUNER_WORD_THIS_IS_HIGH);
      msg = String(msg_cstr);
      mesh.sendBroadcast(msg);
      //
      message = TUNER_WORD_THIS_IS_HIGH;
      reaction_task.restart();
    }
  }
  level_prev = level;
}
Task tune_task(20, TASK_FOREVER, &tune);

//setup_member
void setup_member() {
  //
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
  //
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  //
  runner.addTask(tune_task);
  tune_task.enable();
  //
  runner.addTask(reaction_task);
}
