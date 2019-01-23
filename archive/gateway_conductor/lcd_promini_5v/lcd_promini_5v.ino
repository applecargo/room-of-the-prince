/*
 * a conductor gateway device to the mesh (part1)
 *
 *   - a I2C Slave with LCD screen & Actions controls.
 *   - and this will have a pre-programmed playlist(actionlist/commandlist).
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
#include <LiquidCrystal.h>

//lcd
LiquidCrystal lcd(12, 11, 2, 3, 4, 5);

//i2c
const int i2c_addr = 8;
void receiveEvent(int numBytes) {
  //numBytes : how many bytes received(==available)
  memberCount = Wire.read();
}
void requestEvent() {
  if (newcmd == true) {
    Wire.write(cmdstr);
    newcmd = false;
    cmdsent = true;
  }
  else {
    Wire.write("NONE#SSS@P");
  }
}

//commandlist
const int Ncommands = 3;
String commandlist[Ncommands] = {
  "PLAY#001@A",
  "PLAY#002@A",
  "PLAY#003@A",
};

//task
#include <TaskScheduler.h>
Scheduler taskrunner;
bool state = false;
int cmd_ptr = 0;
void conductor() {
  //DEBUG
  state = !state;

  //queue a i2c request
  commandlist[cmd_ptr].toCharArray(cmdstr, CMD_BUFF_LEN);
  newcmd = true;
  cmdsent = false;

  //
  cmd_ptr += 1;
  if (cmd_ptr >= Ncommands) {
    cmd_ptr = 0;
  }
}
#define CONDUCTOR_INTERVAL 360000 // 6 min -> 360 sec -> 360000 millisec
// #define CONDUCTOR_INTERVAL 1000   // 1 sec. (DEBUG)
Task conductor_task(CONDUCTOR_INTERVAL, TASK_FOREVER, conductor);

//button
const int button_pin = 10;
const int select_pin = 9;
const int analog_pin = 1; //15

//map
float mapping(float val, float from_start, float from_end, float to_start, float to_end) {
  if (val < from_start) return to_start;
  if (val > from_end) return to_end;
  return (val - from_start) * (to_end - to_start) / (from_end - from_start) + to_start;
}

void setup() {
  //lcd
  lcd.begin(16, 2);

  //i2c
  Wire.begin(i2c_addr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //button interrupt
  pinMode(button_pin, INPUT_PULLUP);
  pinMode(select_pin, INPUT_PULLUP);
  pinMode(analog_pin, INPUT_PULLUP); // PULL UP doesn't work?

  //serial monitor
  Serial.begin(115200);

  //task
  taskrunner.init();
  taskrunner.addTask(conductor_task);
  conductor_task.enable();
}

void periodic() {

  //play on/off toggle
  static int select = LOW;
  static int lastSelect = LOW;

  //play on/off toggle
  select = digitalRead(select_pin);
  if (select == HIGH && select != lastSelect) { // rising edge
    conductor_task.enable();
  }
  if (select == LOW && select != lastSelect) { // falling edge
    conductor_task.disable();
  }
  lastSelect = select;

  // lcd
  const char runner[] = {'-', '|'};
  // first line
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("!soundmesh! "); //12
  lcd.print(runner[millis()%2]); // indicator //13
  lcd.print(" "); // 14
  if (memberCount >= 0) {
    lcd.print(memberCount, DEC); // members up to 99 // 16
  }
  else {
    lcd.print("?"); // unknown // 16
  }
  // second line
  lcd.setCursor(0, 1);
  if (select == HIGH) {
    lcd.print("ON AIR!");   // 7
    lcd.print("    "); // 11
  }
  else {
    lcd.print("SLEEPING...");   // 11
  }
  lcd.print((state ? "O" : "X")); // 12 - DEBUG, monitoring if task is running or not.
}

void loop() {
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 100) { // every 100ms
    //
    periodic();
    //
    lastMillis = millis();
  }

  taskrunner.execute();
}
