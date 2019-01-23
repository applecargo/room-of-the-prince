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
#include <Firmata.h>
#include <painlessMesh.h>

//defines
#define MESH_SSID "soundmesh"
#define MESH_PASSWORD "timemachine999"
#define MESH_PORT 5555
#define MESH_CHANNEL 5

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

//firmata
void analogWriteCallback(byte pin, int value){
  if (pin == 10) {
    //DEBUG
    //sprintf(cmdstr, "p:%d,v:%d", pin, value);

    //COMMAND
    if (value == 0) { // 0 - STOP
      sprintf(cmdstr, "STOP#000@A", value);
      //convert to String
      String msg = String(cmdstr);
      //check if valid command
      if (msg.substring(0,4).equals("NONE") == false) {
        //broadcast msg(==cmdstr) to the mesh
        mesh.sendBroadcast(msg);
      }
    }
    else {
      sprintf(cmdstr, "PLAY#%03d@A", value);
      //convert to String
      String msg = String(cmdstr);
      //check if valid command
      if (msg.substring(0,4).equals("NONE") == false) {
        //broadcast msg(==cmdstr) to the mesh
        mesh.sendBroadcast(msg);
      }
    }
  }
}

void setup(void) {

  //serial
  Serial.begin(115200);

  //firmata
  Firmata.setFirmwareVersion(0,1);
  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
  Firmata.begin(57600);

  //mesh
  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);
  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, MESH_CHANNEL);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);
  //callbacks
  mesh.onReceive(&receivedCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
}


void loop() {
  while (Firmata.available()) {
    Firmata.processInput();
  }
}
