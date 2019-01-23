//
// Exhibition @ exhibition-space
//   <one and twelve one-hundred-eighth seconds at the prince's room>
//
// Feb. 11 @ 2019
//

// the common sense
#include "common.h"

// the members lounge
#if (IDENTITY == ID_MOTION_SENSOR)
#include "../members/motion.cpp"
#elif (IDENTITY == ID_RELAY_CTRLER)
#include "../members/relay.cpp"
#elif (IDENTITY == ID_POINT_MOTOR)
#include "../members/pointer.cpp"
#endif

// painless mesh
painlessMesh mesh;

//scheduler
Scheduler runner;

//task #0 : connection indicator
bool onFlag = false;
//prototypes
void taskStatusBlink_steadyOn();
void taskStatusBlink_slowblink_insync();
void taskStatusBlink_fastblink();
//the task
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

// mesh callbacks
void receivedCallback(uint32_t from, String & msg) { // REQUIRED
  // let the device know.
  gotMessageCallback(from, msg);
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
    Serial.println("connected!");
  }
  else {
    // disconnected!!
    statusblinks.set(0, 1, &taskStatusBlink_steadyOn);
    statusblinks.enable();
    // Serial.println("disconnected!");
  }
  // let the device know.
  gotChangedConnectionCallback();
}
void newConnectionCallback(uint32_t nodeId) {
  changedConnectionCallback();
}

void setup_member();
void setup() {
  //led
  pinMode(LED_PIN, OUTPUT);

  //mesh
  // mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);
  mesh.setDebugMsgTypes( ERROR | STARTUP );
  mesh.init(MESH_SSID, MESH_PASSWORD, &runner, MESH_PORT, WIFI_AP_STA, MESH_CHANNEL);
#ifdef MESH_ANCHOR
  mesh.setContainsRoot(true);
#endif
  //callbacks
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  //tasks
  runner.addTask(statusblinks);
  statusblinks.enable();

  //serial
  Serial.begin(9600);
  Serial.println("setup done.");

  //setup_member
  setup_member();
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
