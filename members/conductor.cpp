//oled (128x32)
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  32
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//tasks
extern Task button_task;
extern Task saying_greeting;

// room protocol
static int message = 0;
static char msg_cstr[MSG_LENGTH_MAX] = {0, };
extern Task reaction_task;
void gotChangedConnectionCallback() { // REQUIRED
}
void gotMessageCallback(uint32_t from, String & msg) { // REQUIRED
  Serial.println(msg);
}

// some reaction for received msg.
void reaction() {
}
Task reaction_task(10, 16, &reaction);

// saying hello
void greeting() {
  static String greeting = "An-nyung, nan na-da. Nae-mal jal duel-oe!";
  String greeting_r = greeting.substring(0, random(1, greeting.length()));
  mesh.sendBroadcast(greeting_r);
}
Task saying_greeting(1000, TASK_FOREVER, &greeting);

// button
static int states = 0;
void button() {
  static int button_prev = HIGH;
  static String msg = "";
  //
  int button = digitalRead(D6);
  if (button != button_prev && button == LOW) {
    Serial.println("oh! the button triggers!");
    // compose the message and send!
    //sprintf(msg_cstr, "[%06d:%03d] To lookat: look around now!", ID_LOOK_AT, LOOKAT_WORD_LOOK_AROUND);
    //sprintf(msg_cstr, "[%06d:%03d] To thunder: go rrrrrrrrr now!", ID_THUNDER, THUNDER_WORD_RRRRR);
    // if (states == 0) {
    //   sprintf(msg_cstr, "[%06d:%03d] To bag: handle up now!", ID_BAG, BAG_WORD_HANDLE_UP);
    // } else if (states == 1) {
    //   sprintf(msg_cstr, "[%06d:%03d] To bag: handle down now!", ID_BAG, BAG_WORD_HANDLE_DOWN);
    // } else if (states == 2) {
    //   sprintf(msg_cstr, "[%06d:%03d] To bag: sing now!", ID_BAG, BAG_WORD_SING);
    // }
    // states++;
    // if (states == 3) {
    //   states = 0;
    // }
    sprintf(msg_cstr, "[%06d:%03d] To harmonica: harmonica time! ha-mororo~~", ID_HARMONICA, HARMONICA_WORD_PLAY_START);
    sprintf(msg_cstr, "[%06d:%03d] To fur: speak out now!", ID_FUR, FUR_WORD_SING);
    sprintf(msg_cstr, "[%06d:%03d] To keyholder: now falling falling!", ID_KEYHOLDER, KEYHOLDER_WORD_FALLING_KEYS);

    msg = String(msg_cstr);
    mesh.sendBroadcast(msg);
    // also show to the display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("now, i ve sent..");
    display.println(msg_cstr);
    display.display();
  }
  //
  button_prev = button;
}
//the task
Task button_task(20, TASK_FOREVER, &button);

//setup_member
void setup_member() {

  //D1, D2 reserved for OLED

  //D4 is reserved for blinking LED of MESH status

  //button
  pinMode(D6, INPUT_PULLUP);

  //ssd
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();

  //tasks
  //NOTE: runner.init() is already done at main.cpp by painlessMesh, so NEVER do that again!
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  //
  runner.addTask(button_task);
  button_task.enable();
  //
  runner.addTask(reaction_task);
}
