// this device has a OLED screen (128x32)

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// room protocol
void gotMessageCallback(uint32_t from, String & msg) { // REQUIRED
  // Serial.println("RX:" + msg);
}
void gotChangedConnectionCallback() { // REQUIRED
  ;
}

// task #1 : my stuff
// let's blindly say hello to everyone.
void greeting() {
  static String greeting = "An-nyung, nan na-da. Nae-mal jal duel-oe!";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  // Serial.println("TX:" + greeting_r);
  mesh.sendBroadcast(greeting_r);
}
//the task
Task saying_greeting(1000, TASK_FOREVER, &greeting);

// bang sender.
static char bang_msg[MSG_LENGTH_MAX] = {0, };
void bang() {
  sprintf(bang_msg, "[%06d:%03d] To lookat: look around now!", ID_LOOK_AT, LOOKAT_WORD_LOOK_AROUND);
  String msg = String(bang_msg);
  mesh.sendBroadcast(msg);
  //
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("now, bang_msg is..");
  display.println(bang_msg);
  display.display();
}
//the task
Task bang_task(0, TASK_ONCE, &bang);

// conductor loop.
void conductor_loop() {
  static int button_prev = HIGH;
  int button = digitalRead(D6);
  // trigger
  if (button != button_prev && button == LOW) {
    Serial.println("trigger!");
    // bang_task.enable();
    bang_task.restart();
  }
  //
  button_prev = button;
}
//the task
Task conductor_loop_task(100, TASK_FOREVER, &conductor_loop);

//setup_member
void setup_member() {

  //D1, D2 reserved for OLED

  //bang
  pinMode(D6, INPUT_PULLUP);

  //task scheduler
  //NOTE: runner.init() is already done at main.cpp by painlessMesh, so NEVER do that again!
  //
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  //
  runner.addTask(conductor_loop_task);
  conductor_loop_task.enable();
  //
  runner.addTask(bang_task);

  //ssd
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  //
  display.clearDisplay();
  display.display();
}
