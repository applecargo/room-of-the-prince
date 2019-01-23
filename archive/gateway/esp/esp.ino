/*
 * a gateway device to the mesh (part2)
 *
 *   - a I2C Master that logs in to the mesh.
 *
 *   - soundmesh project @ 2018. 04.
 */

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

//libraries
#include <Wire.h>
#include <painlessMesh.h>

//defines
#define MESH_SSID "soundmesh"
#define MESH_PASSWORD "timemachine999"
#define MESH_PORT 5555
#define MESH_CHANNEL 5

//i2c
const int i2c_addr = 8;

//mesh
painlessMesh mesh;
SimpleList<uint32_t> nodes;
void receivedCallback(uint32_t from, String & msg) {
  // needed to be exist
}
void changedConnectionCallback() {
  nodes = mesh.getNodeList();
  memberCount = nodes.size();
}

void setup() {
  //i2c master
  Wire.begin();

  //mesh
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, MESH_CHANNEL);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);
  //callbacks
  mesh.onReceive(&receivedCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  //serial
  Serial.begin(115200);
}

void periodic() {
  //infrom node size
  Wire.beginTransmission(i2c_addr);
  Wire.write(memberCount);
  Wire.endTransmission();

  //fetch cmdstr
  Wire.requestFrom(i2c_addr, CMD_LENGTH); //request cmdstr.
  if (CMD_LENGTH == Wire.readBytes(cmdstr, CMD_LENGTH)) { // receive cmdstr.
    //convert to String
    String msg = String(cmdstr);
    //check if valid command
    if (msg.substring(0,4).equals("NONE") == false) {
      //broadcast msg(==cmdstr) to the mesh
      mesh.sendBroadcast(msg);
    }
  }
}

void loop() {
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 100) { // every 100ms
    //
    periodic();
    //
    lastMillis = millis();
  }

  //
  mesh.update();
}
