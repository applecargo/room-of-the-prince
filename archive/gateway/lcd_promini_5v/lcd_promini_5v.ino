/*
 * a gateway device to the mesh (part1)
 *
 *   - a I2C Slave with LCD screen & Actions controls.
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
bool composingcmd = false;

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
}

void periodic() {

  // //TEST
  // Serial.print("button_pin : ");
  // Serial.println(digitalRead(button_pin));
  // Serial.print(", select_pin : ");
  // Serial.println(digitalRead(select_pin));
  // Serial.print(", analog_pin : ");
  // Serial.println(analogRead(analog_pin));

  //user interface - compose the command string
  static int button = HIGH;
  static int lastButton = HIGH;
  button = digitalRead(button_pin);
  //
  static int select = 0;
  static int analog = 0;
  //
  select = !digitalRead(select_pin); // a dirty fix! lazy to re-solder.
  analog = analogRead(analog_pin);
  analog = 1023 - analog; // a dirty fix! lazy to re-solder.
  // CCCC#SSS@O : CCCC - commands, SSS - song #, O - output select
  sprintf(cmdstr,
          "%s#%03d@%c",
          (select ? "PLAY" : "STOP"), //4
          (unsigned int)mapping(analog, 0, 1024, 1, 21), //3 - song # range : 001 ~ 020
          'A'); //F : flag objects, C : circular objects, A : all objects

  // //TEST
  // Serial.println(cmdstr);
  //
  // a triggering 'send' button!
  if (button == LOW && button != lastButton) { // falling edge
    newcmd = true;
    cmdsent = false;
  }
  //
  lastButton = button;

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
  lcd.print("CMD:");   // 4
  lcd.print(cmdstr);   // 10
  if (cmdsent == true) {
    lcd.print(":O"); // 2
  }
  else {
    lcd.print(":X"); // 2
  }

  //TEST
  Serial.println("HI?");
}

void loop() {
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 100) { // every 100ms
    //
    periodic();
    //
    lastMillis = millis();
  }
}
