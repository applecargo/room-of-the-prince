//
extern Task motion_msg_task;

//
static int content = 0;

// room protocol
void gotMessageCallback(uint32_t from, String & msg) { // REQUIRED
  // Serial.println("RX:" + msg);
  Serial.println(msg);
}
void gotChangedConnectionCallback() { // REQUIRED
  ;
}

// task #1 : my stuff
// let's blindly say hello to everyone.
void greeting() {
  static String greeting = "Hello? My name is.. Mother Detroit.";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  // Serial.println("TX:" + greeting_r);
  mesh.sendBroadcast(greeting_r);
}
//the task
Task saying_greeting(1000, TASK_FOREVER, &greeting);

// task #2 : detect motion
static char msg_cstr[MSG_LENGTH_MAX] = {0, };
void motion() {
  static bool motion_prev = false;
  String msg;
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
      content = MOTION_WORD_MOTION_START;
      motion_msg_task.restart();
    }
    else
    {
      // Serial.println("motion stop.");
      sprintf(msg_cstr, "[%06d:%03d] To everyone: Uhm, my world is tran·quilo.", ID_EVERYONE, MOTION_WORD_MOTION_START);
      msg = String(msg_cstr);
      mesh.sendBroadcast(msg);
      //
      content = MOTION_WORD_MOTION_END;
      motion_msg_task.restart();
    }
  }
  motion_prev = motion;
}
//the task
Task detect_motion(20, TASK_FOREVER, &motion);

// task #3 : motion. express received msg.
static int mask = 0x8000; // 16bit mask --> iteration of 16 times.
static int tgl_cnt = 0; // 16bit mask --> iteration of 16 times.
// by digital toggling
void digitalToggler() {
  if (motion_msg_task.isFirstIteration()) {
    mask = 0x8000;
  }
  // Serial.print("bit-seq: ");
  // Serial.println((content & mask) == 0);
  if ((content & mask) == 0) {
    //digital ON.
    digitalWrite(D7, HIGH);
  }
  else {
    //digital OFF.
    digitalWrite(D7, LOW);
  }
  //
  mask = mask >> 1;
}
//the task
Task motion_msg_task(100, 16, &digitalToggler);

//setup_member
void setup_member() {
  //
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  //
  runner.addTask(detect_motion);
  detect_motion.enable();
  //
  runner.addTask(motion_msg_task);
}
