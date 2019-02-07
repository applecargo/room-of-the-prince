//
// Exhibition @ exhibition-space
//   <one and twelve one-hundred-eighth seconds at the prince's room>
//
// Feb. 11 @ 2019
//

// the common sense
#include "common.h"

// the members lounge
#if (IDENTITY == ID_CONDUCTOR)
#include "../members/conductor.cpp"
//
#elif (IDENTITY == ID_MOTION_SENSOR)
#include "../members/motion.cpp"
//
#elif (IDENTITY == ID_RELAY_CTRLER)
#include "../members/relay.cpp"
//
#elif (IDENTITY == ID_POINT_MOTOR)
#include "../members/pointer.cpp"
//
#elif (IDENTITY == ID_LOOK_AT)
#include "../members/lookat.cpp"
//
#elif (IDENTITY == ID_THUNDER)
#include "../members/thunder.cpp"
//
#elif (IDENTITY == ID_ROCKING)
#include "../members/rocking.cpp"
//
#elif (IDENTITY == ID_DISTANCE)
#include "../members/distance.cpp"
//
#elif (IDENTITY == ID_BAG)
#include "../members/bag.cpp"
//
#elif (IDENTITY == ID_HARMONICA)
#include "../members/harmonica.cpp"
//
#elif (IDENTITY == ID_FUR)
#include "../members/fur.cpp"
//
#elif (IDENTITY == ID_KEYHOLDER)
#include "../members/keyholder.cpp"
//
#elif (IDENTITY == ID_MIRROR)
#include "../members/mirror.cpp"
//
#elif (IDENTITY == ID_PEAK)
#include "../members/peak.cpp"
//
#elif (IDENTITY == ID_BELL)
#include "../members/bell.cpp"
//
#elif (IDENTITY == ID_FLOAT)
#include "../members/float.cpp"
//
#elif (IDENTITY == ID_HEATER)
#include "../members/heater.cpp"
//
#elif (IDENTITY == ID_GPS)
#include "../members/gps.cpp"
//
#elif (IDENTITY == ID_DOOR)
#include "../members/door.cpp"
//
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
  // let the member device know.
  gotMessageCallback(from, msg);
}
void changedConnectionCallback() {
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
  }
  // let the member device know.
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
  WiFiMode_t node_type = WIFI_AP_STA;
#if (NODE_TYPE == NODE_TYPE_STA_ONLY)
  system_phy_set_max_tpw(0);
  node_type = WIFI_STA;
#endif
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);
  // mesh.setDebugMsgTypes( ERROR | STARTUP );
  mesh.init(MESH_SSID, MESH_PASSWORD, &runner, MESH_PORT, node_type, MESH_CHANNEL);

  //
  // void init(String ssid, String password, Scheduler *baseScheduler, uint16_t port = 5555, WiFiMode_t connectMode = WIFI_AP_STA, uint8_t channel = 1, uint8_t hidden = 0, uint8_t maxconn = MAX_CONN);
  // void init(String ssid, String password, uint16_t port = 5555, WiFiMode_t connectMode = WIFI_AP_STA, uint8_t channel = 1, uint8_t hidden = 0, uint8_t maxconn = MAX_CONN);
  //

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
  delay(100);
  Serial.println("setup done.");
  Serial.print("IDENTITY: ");
  Serial.println(IDENTITY, HEX);
#if (NODE_TYPE == NODE_TYPE_STA_ONLY)
  Serial.println("INFO: we are in the WIFI_STA mode!");
#endif

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
