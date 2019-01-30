// room protocol
void gotMessageCallback(uint32_t from, String & msg) { // REQUIRED
  // Serial.println("RX:" + msg);
  Serial.println(msg);
  if (msg.charAt(0) == '[') {
    if (msg.substring(1, 3) == "M1") {
      Serial.println("M1 happened!");
      digitalWrite(D1, HIGH);
    }
    else if (msg.substring(1, 3) == "M0") {
      Serial.println("M0 happened!");
      digitalWrite(D1, LOW);
    }
  }
}
void gotChangedConnectionCallback() { // REQUIRED
  ;
}

// task #1 : my stuff
// let's blindly say hello to everyone.
void greeting() {
  static String greeting = "Que tal? Me llame Santa Claudius, mi amigo!";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  // Serial.println("TX:" + greeting_r);
  mesh.sendBroadcast(greeting_r);
}
//the task
Task saying_greeting(1000, TASK_FOREVER, &greeting);

//setup_member
void setup_member() {
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  //
  pinMode(D1, OUTPUT);
}
