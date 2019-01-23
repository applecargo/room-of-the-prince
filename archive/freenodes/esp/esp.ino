/*
 * freenode devices to the mesh (part1)
 *
 *   - a I2C Master that logs in to the mesh.
 *
 *   - soundmesh project @ 2018. 04 ~ 06
 */

#define BOARD_NODEMCU_ESP12E 1
#define BOARD_NODEMCU_ESP12N 2
#define BOARD_NODEMCU_ESP32  3

/*
 * Board
 *
 * ESP8266 (nodemcu-esp12e) status ==> Stable.
 *
 * ESP8266 (esp12n) status ==> Stable.
 * Carefully select proper compiler/uploader setup @ arduino
 *
 * ESP32 status ==> Unstable.
 * problem: i2c stabillity. + (mesh stability)
 * trial #1: try stickbreaker's arduino-esp32 which is seriously targeting i2c stability
 * trial #2: try encode/decode command bytes. to be single byte. seems RTOS running under the hood (hal) causing problem with i2c strict timing. so i feel good on single bytes according to people's talks.
 * first thing first.. : confirm pinout. + stickbreaker (if not work/or unstable) + encode/decode in 1 byte. is my best effort. then we might have to consider different way for communication... sadly.
 * a goooood article on esp32 subject.. possibilities.. potentials..!
 *  --> https://mjrobot.org/iot-made-simple-playing-with-the-esp32-on-arduino-ide/
 * pinout confirmation
 * SDA ==> GPIO 21
 * SCL ==> GPIO 22
 * teensy 3.5 default
 * SDA ==> 18
 * SCL ==> 19
 * so,
 * esp32 ==> teensy 3.5
 *  21 <==> 18
 *  22 <==> 19
 * and!
 * NOTE: esp32 i2c function will drive bus pull-up on the fly! Don't use 3.3k pullups at teensy side! otherwise it won't work.
 */
#define BOARD_SELECT BOARD_NODEMCU_ESP12E

/*
 * Network
 *
 * define or undef MESH_ANCHOR : let this node be a 'root'-inclusion obsessive or simply free.
 * anchors should be placed around 'root' to speed up mesh establishment and stabilize mesh integrity.
 */
#define MESH_SSID "soundmesh"
#define MESH_PASSWORD "timemachine999"
#define MESH_PORT 5555
#define MESH_CHANNEL 5
// #define MESH_ANCHOR

// shared global (protocol) : begin
//command (i2c)
#define CMD_LENGTH 10
#define CMD_BUFF_LEN (CMD_LENGTH + 1)
char cmdstr[CMD_BUFF_LEN] = "NONE#SSS@P";
//info (mesh)
char memberCount = -1;      //before any update from the mesh
//flags (events)
bool cmdsent = false;
bool newcmd = false;
// shared global (protocol) : end

//libraries
#include <Wire.h>
#include <painlessMesh.h>

//i2c
const int i2c_addr = 3;

//mesh
painlessMesh mesh;
void receivedCallback(uint32_t from, String & msg) { // needed to be exist
  //i2c
  msg.toCharArray(cmdstr, CMD_BUFF_LEN);

  Wire.beginTransmission(i2c_addr);
  int nb = Wire.write(cmdstr);
  Wire.endTransmission();
}

//tasks
Scheduler runner;
//
// LED status indication
//
// operation modes
//
// 0 - booted. and running. no connection. scanning.
// 1 - + connected.
//
// notifying patterns
//
// 0 - steady on
// 1 - slow blinking (syncronized)
//
#define LED_PIN 2 // built-in LED
#define LED_PERIOD 1000
#define LED_ONTIME 100
bool onFlag = false;
Task statusblinks(0, 1, &taskStatusBlink_steadyOn); // at start, steady on. default == disabled. ==> setup() will enable.
// when disconnected, steadyon.
void taskStatusBlink_steadyOn() {
  onFlag = true;
}
// blink per 1s. sync-ed.
void taskStatusBlink_slowblink_insync() {
  // toggler
  if (onFlag) {
    onFlag = false;
  }
  else {
    onFlag = true;
  }
  // on-time
  statusblinks.delay(LED_ONTIME);
  // re-enable & sync.
  if (statusblinks.isLastIteration()) {
    statusblinks.setIterations(2); //refill iteration counts
    statusblinks.enableDelayed(LED_PERIOD - (mesh.getNodeTime() % (LED_PERIOD*1000))/1000); //re-enable with sync-ed delay
  }
}
void taskStatusBlink_fastblink() {
}

void changedConnectionCallback() {
  // Serial.println("changed connection");
  // Serial.print("mesh.getNodeList().size():");
  // Serial.println(mesh.getNodeList().size());

  // check status -> modify status LED
  if (mesh.getNodeList().size() > 0) {
    // (still) connected.
    onFlag = false; //reset flag stat.
    statusblinks.set(LED_PERIOD, 2, &taskStatusBlink_slowblink_insync);
    statusblinks.enable();
    // Serial.println("connected!");
  }
  else {
    // disconnected!!
    statusblinks.set(0, 1, &taskStatusBlink_steadyOn);
    statusblinks.enable();
    // Serial.println("disconnected!");
  }
}

void setup() {
  //i2c master
  Wire.begin();

  //led
  pinMode(LED_PIN, OUTPUT);

  //mesh
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);
  mesh.init(MESH_SSID, MESH_PASSWORD, &runner, MESH_PORT, WIFI_AP_STA, MESH_CHANNEL);
#ifdef MESH_ANCHOR
  mesh.setContainsRoot(true);
#endif
  //callbacks
  mesh.onReceive(&receivedCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  //tasks
  runner.addTask(statusblinks);
  statusblinks.enable();

  //serial
  Serial.begin(9600);
}

void loop() {
  runner.execute();
  mesh.update();
#if (BOARD_SELECT == BOARD_NODEMCU_ESP32)
  digitalWrite(LED_PIN, onFlag); // value == true is ON.
#else
  digitalWrite(LED_PIN, !onFlag); // value == false is ON. so onFlag == true is ON. (pull-up)
#endif
}
