/*
 * drumnode devices to the mesh (part2)
 *
 *   - a I2C Slave and a sounder device.
 *
 *   - soundmesh project @ 2018. 04.
 */

//problems:
//  enhancement on i2c protocol: start byte, CRC etc.

#define DRUMMING_AUTO_TEST false
#define DRUMMING_AUTO_TEST_INTERVAL 5000

// shared global (protocol) : begin
//command (i2c)
#define CMD_LENGTH 10
#define CMD_BUFF_LEN (CMD_LENGTH + 1)
char cmdstr[CMD_BUFF_LEN] = "NONE#SSS@P";
//info (mesh)
char memberCount = -1;       //before any update from the mesh
//flags (events)
bool cmdsent = false;
bool newcmd = false;
// shared global (protocol) : end

//set my group
String mygroup = "D"; //F : flag objects, C : circular objects, D : drumming objects, A : all objects

//servo
#include <Servo.h>
Servo left_drum;
Servo right_drum;

//task
#include <TaskScheduler.h>
Scheduler runner;
#define HIT_ANGLE_LEFT 130
#define REST_ANGLE_LEFT 40
#define HIT_ANGLE_RIGHT 40
#define REST_ANGLE_RIGHT 130
#define HIT_DELAY 100
//callback prototypes
void left_drum_hit();
void left_drum_rest();
void right_drum_hit();
void right_drum_rest();
void drumming_test();
void drumming_scheduler();
//tasks
Task left_drum_task(0, TASK_ONCE, left_drum_rest);
Task right_drum_task(0, TASK_ONCE, right_drum_rest);
#if (DRUMMING_AUTO_TEST == true)
Task drumming_test_task(DRUMMING_AUTO_TEST_INTERVAL, TASK_FOREVER, drumming_test, &runner, true);
#endif
Task drumming_scheduler_task(0, TASK_ONCE, drumming_scheduler);

//callbacks
void left_drum_hit() {
  left_drum.write(HIT_ANGLE_LEFT);
  left_drum_task.setCallback(left_drum_rest);
  left_drum_task.restartDelayed(HIT_DELAY);
}
void left_drum_rest() {
  left_drum.write(REST_ANGLE_LEFT);
}
void right_drum_hit() {
  right_drum.write(HIT_ANGLE_RIGHT);
  right_drum_task.setCallback(right_drum_rest);
  right_drum_task.restartDelayed(HIT_DELAY);
}
void right_drum_rest() {
  right_drum.write(REST_ANGLE_RIGHT);
}
void drumming_test() {
  //play left drum once.
  left_drum_task.setCallback(left_drum_hit);
  left_drum_task.restart();
  //play right drum once.
  right_drum_task.setCallback(right_drum_hit);
  right_drum_task.restartDelayed(200);
}
String playing_song = "";
int hitcount = 0;
bool isstarted = true;
void drumming_scheduler() {

  //DEBUG
  Serial.print("playing_song => ");
  Serial.print(playing_song);
  Serial.print(", hitcount => ");
  Serial.println(hitcount);

  //
  int del;

  //song 001 play score : "crickets"
  if (playing_song == "001") {

    //starting silence.
    if (isstarted) {
      isstarted = false;
      del = 45000; //start after 45 sec.
      drumming_scheduler_task.restartDelayed(del);
      //DEBUG
      Serial.print("start-up scheduled: restartDelayed => ");
      Serial.println(del);
      return;
    }

    //a single
    left_drum_task.setCallback(left_drum_hit);
    left_drum_task.restart();

    //fixed speed.
    del = 10000;

    //stop cond.
    hitcount += 1;
    if (hitcount < 10) { // 10 times.
      drumming_scheduler_task.restartDelayed(del);
      //DEBUG
      Serial.print("scheduled: restartDelayed => ");
      Serial.println(del);
    }
  }

  //song 002 play score : "round song"
  else if (playing_song == "002") {

    //starting silence.
    if (isstarted) {
      isstarted = false;
      del = 60000; //start after 1 min.
      drumming_scheduler_task.restartDelayed(del);
      //DEBUG
      Serial.print("start-up scheduled: restartDelayed => ");
      Serial.println(del);
      return;
    }

    //DON't play any.

  }

  //song 003 play score : "construction"
  else if (playing_song == "003") {

    //starting silence.
    if (isstarted) {
      isstarted = false;
      del = 50000; //start in 50 sec.
      drumming_scheduler_task.restartDelayed(del);
      //DEBUG
      Serial.print("start-up scheduled: restartDelayed => ");
      Serial.println(del);
      return;
    }

    //a double
    // left_drum_task.setCallback(left_drum_hit);
    // left_drum_task.restart();
    // right_drum_task.setCallback(right_drum_hit);
    // right_drum_task.restartDelayed(500);
    // //increasing speed
    // del = 20000 - hitcount * 1000;
    // if (del < 5000) {
    //   del = 5000;
    // }
    // // (!) 20 (!) 19 (!) 17 (!) 14 (!) 10 (!) 5 (!) 5 (!) 5 (!) 5 (!) 5 (!) 5 (!) ~ 110 seconds

    //alternative drumming
    if (hitcount % 2 == 0) {
      left_drum_task.setCallback(left_drum_hit);
      left_drum_task.restart();
    }
    else {
      right_drum_task.setCallback(right_drum_hit);
      right_drum_task.restart();
    }
    //increasing speed
    int d = hitcount / 15;
    switch (d) {
    case 0:
      del = 2000; // 2 sec x 15 = 30
      break;
    case 1:
      del = 1800; // 1.8 sec x 15 = 27
      break;
    case 2:
      del = 1500; // 1.5 sec x 15 = 22.5
      break;
    case 3:
      del = 1000; // 1 sec x 15 = 15
      break;
    default:
      del = 1000; //above == 94.5 sec. (60 times) + 15.5 sec. (15 times) more.
    }

    //stop cond.
    hitcount += 1;
    if (hitcount < 75) { // 75 times.
      drumming_scheduler_task.restartDelayed(del);
      //DEBUG
      Serial.print("scheduled: restartDelayed => ");
      Serial.println(del);
    }
  }
}

//i2c
#include <Wire.h>
const int i2c_addr = 3;
void receiveEvent(int numBytes) {
  //numBytes : how many bytes received(==available)

  // Serial.println("[i2c] on receive!");
  int nb = Wire.readBytes(cmdstr, CMD_LENGTH);
  Serial.print("[i2c] cmdstr : ");
  Serial.println(cmdstr);

  if (CMD_LENGTH == nb) { // receive cmdstr.

    //convert to String
    String msg = String(cmdstr);

    //parse command string.
    String cmd = msg.substring(0,4); // 0123
    String song = msg.substring(5,8); // 567
    String group = msg.substring(9,10); // 9

    //process commands
    if (group == mygroup || group == "A") {

      if (cmd.equals("NONE")) {}
      else if (cmd.equals("PLAY")) {
        playing_song = song;
        hitcount = 0;
        isstarted = true;

        //DEBUG
        Serial.print("[receiveEvent => scheduler start-up] playing_song => ");
        Serial.print(playing_song);
        Serial.print(", hitcount => ");
        Serial.println(hitcount);
        drumming_scheduler_task.restart();
      }
      else if (cmd.equals("STOP")) {
        //invalid song name will gracefully stop the scheduler
        playing_song = "";
        hitcount = 0;
        isstarted = true;
        drumming_scheduler_task.restart();
      }
    }
  }
}

void setup() {
  //serial monitor
  Serial.begin(115200);
  delay(50);

  //i2c
  //NOTE: But, don't use pull-up registers for esp boards. -> esp i2c function will drive bus pull-up on the fly!
  // pinMode(15, OUTPUT);
  // digitalWrite(15, HIGH); // for i2c pullup
  // pinMode(22, OUTPUT);
  // digitalWrite(22, HIGH); // for i2c pullup
  Wire.begin(i2c_addr);
  Wire.onReceive(receiveEvent);

  //led
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // LOW: OFF

  //servo - drums
  left_drum.attach(9);
  right_drum.attach(10);

  //player task
  runner.addTask(left_drum_task);
  runner.addTask(right_drum_task);
  runner.addTask(drumming_scheduler_task);
  left_drum_task.enable();
  right_drum_task.enable();
  drumming_scheduler_task.enable();

  //
  Serial.println("[setup] done.");
}

void loop() {
  runner.execute();
}
