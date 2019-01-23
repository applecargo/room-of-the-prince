/*
 * a gateway device to the mesh (part1)
 *
 *   - a I2C Slave with TFTLCD+Touch screen & Actions controls.
 *
 *   - soundmesh project @ 2018. 08.
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
#include <Firmata.h>

//tftlcd_touch
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif
#ifndef USE_ADAFRUIT_SHIELD_PINOUT
 #error "This sketch is intended for use with the TFT LCD Shield. Make sure that USE_ADAFRUIT_SHIELD_PINOUT is #defined in the Adafruit_TFTLCD.h library file."
#endif
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin
#ifdef __SAM3X8E__
  #define TS_MINX 125
  #define TS_MINY 170
  #define TS_MAXX 880
  #define TS_MAXY 940
#else
  #define TS_MINX  150
  #define TS_MINY  120
  #define TS_MAXX  920
  #define TS_MAXY  940
#endif
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
Adafruit_TFTLCD tft;
#define MINPRESSURE 10
#define MAXPRESSURE 1000

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

//map
float mapping(float val, float from_start, float from_end, float to_start, float to_end) {
  if (val < from_start) return to_start;
  if (val > from_end) return to_end;
  return (val - from_start) * (to_end - to_start) / (from_end - from_start) + to_start;
}

//firmata
void analogWriteCallback(byte pin, int value){
  if (pin == 10) {
    //DEBUG
    //sprintf(cmdstr, "p:%d,v:%d", pin, value);

    //COMMAND
    if (value == 0) { // 0 - STOP
      sprintf(cmdstr, "STOP#000@A", value);
      newcmd = true;
      cmdsent = false;
    }
    else {
      sprintf(cmdstr, "PLAY#%03d@A", value);
      newcmd = true;
      cmdsent = false;
    }
  }
}

void setup(void) {

  //serial
  Serial.begin(115200);

  //i2c
  Wire.begin(i2c_addr);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //tftlcd_touch
  tft.reset();
  tft.begin(tft.readID());
  pinMode(13, OUTPUT); //enable/disable touch by pin 13
  tft.setRotation(3);
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE, BLACK);

  //firmata
  Firmata.setFirmwareVersion(0,1);
  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
  Firmata.begin(57600);

}

int button_r = 20;
int button1_posX = 30;
int button1_posY = 130;
int button2_posX = 100;
int button2_posY = 130;
int button3_posX = 170;
int button3_posY = 130;
int button4_posX = 30;
int button4_posY = 180;
int button5_posX = 100;
int button5_posY = 180;
int button6_posX = 170;
int button6_posY = 180;
int buttonSTOP_posX = 240;
int buttonSTOP_posY = 155;
void periodic() {

  //tftlcd_touch

  //enable & get touch values
  digitalWrite(13, HIGH);
  pinMode(XM, INPUT); //shared pin direction re-set (OUTPUT -> INPUT)
  pinMode(YP, INPUT); //shared pin direction re-set (OUTPUT -> INPUT)
  TSPoint p = ts.getPoint();

  //disable & set screen changes
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT); //shared pin direction re-set (INPUT -> OUTPUT)
  pinMode(YP, OUTPUT); //shared pin direction re-set (INPUT -> OUTPUT)

  //touch event handling
  // Serial.println("pressure: " + String(p.z));
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // for rotation == 0
    // p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    // p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
    //lcd rotation fix. + scale to match. (lcd <--> touch)
    int16_t temp = p.x;
    p.x = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
    p.y = map(temp, TS_MINX, TS_MAXX, tft.height(), 0);

    Serial.println("(" + String(p.x) + ", " + String(p.y) + ")");

    //button 1
    if (p.x > button1_posX - button_r && p.x < button1_posX + button_r) {
      if (p.y > button1_posY - button_r && p.y < button1_posY + button_r) {
        sprintf(cmdstr, "PLAY#001@A");
        newcmd = true;
        cmdsent = false;
      }
    }
    //button 2
    if (p.x > button2_posX - button_r && p.x < button2_posX + button_r) {
      if (p.y > button2_posY - button_r && p.y < button2_posY + button_r) {
        sprintf(cmdstr, "PLAY#002@A");
        newcmd = true;
        cmdsent = false;
      }
    }
    //button 3
    if (p.x > button3_posX - button_r && p.x < button3_posX + button_r) {
      if (p.y > button3_posY - button_r && p.y < button3_posY + button_r) {
        sprintf(cmdstr, "PLAY#003@A");
        newcmd = true;
        cmdsent = false;
      }
    }
    //button 4
    if (p.x > button4_posX - button_r && p.x < button4_posX + button_r) {
      if (p.y > button4_posY - button_r && p.y < button4_posY + button_r) {
        sprintf(cmdstr, "PLAY#004@A");
        newcmd = true;
        cmdsent = false;
      }
    }
    //button 5
    if (p.x > button5_posX - button_r && p.x < button5_posX + button_r) {
      if (p.y > button5_posY - button_r && p.y < button5_posY + button_r) {
        sprintf(cmdstr, "PLAY#005@A");
        newcmd = true;
        cmdsent = false;
      }
    }
    //button 6
    if (p.x > button6_posX - button_r && p.x < button6_posX + button_r) {
      if (p.y > button6_posY - button_r && p.y < button6_posY + button_r) {
        sprintf(cmdstr, "PLAY#006@A");
        newcmd = true;
        cmdsent = false;
      }
    }
    //button STOP
    if (p.x > buttonSTOP_posX - button_r && p.x < buttonSTOP_posX + button_r) {
      if (p.y > buttonSTOP_posY - button_r && p.y < buttonSTOP_posY + button_r) {
        sprintf(cmdstr, "STOP#SSS@A");
        newcmd = true;
        cmdsent = false;
      }
    }
  }

  // // CCCC#SSS@O : CCCC - commands, SSS - song #, O - output select
  // sprintf(cmdstr,
  //         "%s#%03d@%c",
  //         (1 ? "PLAY" : "STOP"), //4
  //         (unsigned int)mapping(100, 0, 1024, 1, 21), //3 - song # range : 001 ~ 020
  //         'A'); //F : flag objects, C : circular objects, A : all objects

  // //
  // static int select = 0;
  // static int analog = 0;
  // //
  // select = !digitalRead(select_pin); // a dirty fix! lazy to re-solder.
  // analog = analogRead(analog_pin);
  // analog = 1023 - analog; // a dirty fix! lazy to re-solder.

  // //TEST
  // Serial.println(cmdstr);
  //
  // a triggering 'send' button!
  // if (button == LOW && button != lastButton) { // falling edge
  //   newcmd = true;
  //   cmdsent = false;
  // }
  //
  // lastButton = button;

  // tftlcd
  const char runner[] = {'-', '|'};
  // first line
  tft.setCursor(0, 0);
  tft.print("!Soundmesh!");
  tft.print(runner[millis()%2]); // indicator
  tft.print(" "); // 14
  if (memberCount >= 0) {
    tft.print(memberCount, DEC); // members up to 99
  }
  else {
    tft.print("?"); // unknown // 16
  }
  // second line
  tft.setCursor(0, 60);
  tft.print("CMD:");   // 4
  tft.print(cmdstr);   // 10
  if (cmdsent == true) {
    tft.print(":O"); // 2
  }
  else {
    tft.print(":X"); // 2
  }

  // widget
  tft.fillCircle(button1_posX, button1_posY, button_r, YELLOW);
  tft.fillCircle(button2_posX, button2_posY, button_r, GREEN);
  tft.fillCircle(button3_posX, button3_posY, button_r, BLUE);
  tft.fillCircle(button4_posX, button4_posY, button_r, CYAN);
  tft.fillCircle(button5_posX, button5_posY, button_r, MAGENTA);
  tft.fillCircle(button6_posX, button6_posY, button_r, WHITE);
  tft.fillCircle(buttonSTOP_posX, buttonSTOP_posY, button_r, RED);
}

void loop() {
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 100) { // every 100ms
    //
    periodic();
    //
    lastMillis = millis();
  }

  while (Firmata.available()) {
    Firmata.processInput();
  }
}
