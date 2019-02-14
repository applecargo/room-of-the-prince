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
Task reaction_task(10, 17, &reaction);

// saying hello
void greeting() {
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", memberList[random(NUM_OF_MEMBERS)], CONDUCTOR_WORD_HELLO); //"An-nyung, nan na-da. Nae-mal jal duel-oe!"
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
}
Task saying_greeting(10000, TASK_FOREVER, &greeting);

//
void compose_send_display_msg()
{
  //
  static int states = 0;
  static String msg = "";

  // compose the message and send!
  switch(states) {
  case 0:
    sprintf(msg_cstr, "[%06d:%03d] To lookat: look around now!", ID_LOOK_AT, LOOKAT_WORD_LOOK_AROUND);
    break;
  case 1:
    sprintf(msg_cstr, "[%06d:%03d] To thunder: go rrrrrrrrr now!", ID_THUNDER, THUNDER_WORD_RRRRR);
    break;
  case 2:
    sprintf(msg_cstr, "[%06d:%03d] To bag: handle up now!", ID_BAG, BAG_WORD_HANDLE_UP);
    break;
  case 3:
    sprintf(msg_cstr, "[%06d:%03d] To bag: handle down now!", ID_BAG, BAG_WORD_HANDLE_DOWN);
    break;
  case 4:
    sprintf(msg_cstr, "[%06d:%03d] To bag: sing now!", ID_BAG, BAG_WORD_SING);
    break;
  case 5:
    sprintf(msg_cstr, "[%06d:%03d] To harmonica: harmonica time! ha-mororo~~", ID_HARMONICA, HARMONICA_WORD_PLAY_START);
    break;
  case 6:
    sprintf(msg_cstr, "[%06d:%03d] To fur: speak out now!", ID_FUR, FUR_WORD_SING);
    break;
  case 7:
    sprintf(msg_cstr, "[%06d:%03d] To keyholder: now falling falling!", ID_KEYHOLDER, KEYHOLDER_WORD_FALLING_KEYS);
    break;
  case 8:
    sprintf(msg_cstr, "[%06d:%03d] To mirror: show me, please!", ID_MIRROR, MIRROR_WORD_IWILLSHOWYOU);
    break;
  case 9:
    sprintf(msg_cstr, "[%06d:%03d] To peak: signal signal!", ID_PEAK, PEAK_WORD_PPI_PPI_PPI);
    break;
  case 10:
    sprintf(msg_cstr, "[%06d:%03d] To bell: ring your bell!", ID_BELL, BELL_WORD_RING_RING_RING);
    break;
  case 11:
    sprintf(msg_cstr, "[%06d:%03d] To float: turn turn and float!", ID_FLOAT, FLOAT_WORD_TURN_TURN);
    break;
  case 12:
    sprintf(msg_cstr, "[%06d:%03d] To heater: again noisy?!", ID_HEATER, HEATER_WORD_NOISY_NOISY );
    break;
  case 13:
    sprintf(msg_cstr, "[%06d:%03d] To rocking: time to rock!!", ID_ROCKING, ROCKING_WORD_ROCK_ROCK_ROCK);
    break;
  case 14:
    sprintf(msg_cstr, "[%06d:%03d] To hanger: you, too, amigo!", ID_HANGER, HANGER_WORD_SING);
    break;
  default:
    ; // undefined
  }
  //
  states++;
  if (states == 15) {
    states = 0;
  }
  //
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
  // also show to the display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(msg_cstr);
  display.display();
}

// button
void button() {
  static int button_prev = HIGH;
  //
  int button = digitalRead(D6);
  if (button != button_prev && button == LOW) {
    Serial.println("oh! the button triggers!");
    compose_send_display_msg();
  }
  //
  button_prev = button;
}
//the task
Task button_task(20, TASK_FOREVER, &button);

//looping events
void words_looping() {
  //
  compose_send_display_msg();
}
Task words_looping_task(10000, TASK_FOREVER, &words_looping);

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
  // runner.addTask(words_looping_task);
  // words_looping_task.enable();
  //
  runner.addTask(reaction_task);
}
