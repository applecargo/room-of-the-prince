/*
 * freenode devices to the mesh (part2)
 *
 *   - a I2C Slave and a sounder device.
 *
 *   - soundmesh project @ 2018. 04.
 *   - adding a motor on/off function. @ 2018. 06.
 */

//NOTE:Potential enhancements:
//  enhancement on i2c protocol: start byte, CRC etc.
//  fade-in/out when track change while isPlaying
//  fade-out when track stops before actual end of track

#define BOARD_TEENSY_35 1
#define BOARD_TEENSY_32 2
/*
 * Board
 *
 */
#define BOARD_SELECT BOARD_TEENSY_35

/*
 * My Group
 *
 * sub-group in the mesh
 * F : flag objects
 * C : circular objects
 * D : drummer objects
 *
 * all members in the mesh
 * A : all objects
 */
String mygroup = "C";

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

//HACK: let auto-poweroff speakers stay turned ON! - (creative muvo mini)
#define IDLE_FREQ 22000
#define IDLE_AMP 0.005

//teensy audio
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#if (BOARD_SELECT == BOARD_TEENSY_35)
//teensy 3.5 with SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

// GUItool: begin automatically generated code
AudioPlaySdWav playSdWav1;               //xy=447,311
AudioSynthWaveformSine sine1;            //xy=459,407
AudioMixer4 mixer2;                      //xy=720,374
AudioMixer4 mixer1;                      //xy=722,291
AudioOutputAnalogStereo dacs1;           //xy=1002,308
AudioConnection patchCord1(playSdWav1, 0, mixer1, 0);
AudioConnection patchCord2(playSdWav1, 1, mixer2, 0);
AudioConnection patchCord3(sine1, 0, mixer1, 1);
AudioConnection patchCord4(sine1, 0, mixer2, 1);
AudioConnection patchCord5(mixer2, 0, dacs1, 1);
AudioConnection patchCord6(mixer1, 0, dacs1, 0);
// GUItool: end automatically generated code
#elif (BOARD_SELECT == BOARD_TEENSY_32)
//teensy 3.2 with SD card custom
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11  // not actually used - ?
#define SDCARD_SCK_PIN   14  // not actually used - ?

// GUItool: begin automatically generated code
AudioPlaySdWav playSdWav1;               //xy=398,327
AudioSynthWaveformSine sine1;            //xy=410,423
AudioMixer4 mixer1;                      //xy=658,383
AudioOutputAnalog dac1;                  //xy=884,382
AudioConnection patchCord1(playSdWav1, 0, mixer1, 0);
AudioConnection patchCord2(sine1, 0, mixer1, 1);
AudioConnection patchCord3(mixer1, dac1);
// GUItool: end automatically generated code
#endif


//motor action (fan)
#define MOTOR_PIN 3

//task
#include <TaskScheduler.h>
Scheduler runner;
//song #
int song_now = 0; //0~99
//
void sound_player_start()
{
  //filename buffer - 8.3 naming convension! 8+1+3+1 = 13
  char filename[13] = "NN.WAV";
  //search for the sound file
  int limit = (song_now % 100);       // 0~99
  filename[0] = '0' + (limit / 10);       // [N]N.WAV
  filename[1] = '0' + (limit % 10);       // N[N].WAV
  //TEST
  Serial.println(filename);
  //start the player!
  //NOTE: block out 're-triggering'
  if (playSdWav1.isPlaying() == false) {
    playSdWav1.play(filename);
  }
  //mark the indicator : HIGH: ON
  digitalWrite(13, HIGH);
  //fan action
  digitalWrite(MOTOR_PIN, HIGH);
  //to wait a bit for updating isPlaying()
  delay(10);
}
void sound_player_stop() {
  //stop the player.
  if (playSdWav1.isPlaying() == true) {
    playSdWav1.stop();
    //fan stop
    digitalWrite(MOTOR_PIN, LOW);
  }
}
void sound_player_check() {
  if (playSdWav1.isPlaying() == false) {
    //mark the indicator : LOW: OFF
    digitalWrite(13, LOW);
    //let speaker leave turned ON!
    sine1.amplitude(IDLE_AMP);
    //fan stop
    digitalWrite(MOTOR_PIN, LOW);
  }
  else {
    //let speaker leave turned ON!
    sine1.amplitude(0);
  }
}
//
Task sound_player_start_task(0, TASK_ONCE, sound_player_start);
Task sound_player_stop_task(0, TASK_ONCE, sound_player_stop);
Task sound_player_check_task(0, TASK_FOREVER, sound_player_check, &runner, true);

//i2c
#include <Wire.h>
const int i2c_addr = 3;
void receiveEvent(int numBytes) {
  //numBytes : how many bytes received(==available)

  // Serial.println("[i2c] on receive!");
  int nb = Wire.readBytes(cmdstr, CMD_LENGTH);
  Serial.print("[i2c] cmdstr : ");
  Serial.println(cmdstr);
  // Serial.print("[i2c] nb : ");
  // Serial.println(nb);

  if (CMD_LENGTH == nb) { // receive cmdstr.

    //convert to String
    String msg = String(cmdstr);

    //parse command string.
    String cmd = msg.substring(0,4); // 0123
    String song = msg.substring(5,8); // 567
    String group = msg.substring(9,10); // 9

    //process commands
    if (group == mygroup || group == "A") {

      if (cmd.equals("NONE")) {
        // //TEST
        // Serial.println("[i2c] none recognized.");
        // Serial.println("[i2c] song: " + song);
        // Serial.println("[i2c] group: " + group);

        //do action
        //nothing to do.
      }
      else if (cmd.equals("PLAY")) {
        // //TEST
        // Serial.println("[i2c] play recognized.");
        // Serial.println("[i2c] song: " + song);
        // Serial.println("[i2c] group: " + group);

        //do action
        song_now = song.toInt();
        if (song_now > 0) {       // 0 is a parsing error
          sound_player_start_task.restart();
        }
        else {
          //error!
          Serial.println("[i2c] parsing error of 'song' parameter!");
        }
      }
      else if (cmd.equals("STOP")) {
        // //TEST
        // Serial.println("[i2c] stop recognized.");
        // Serial.println("[i2c] song: " + song);
        // Serial.println("[i2c] group: " + group);

        //do action
        sound_player_stop_task.restart();

        //TO DO : maybe fade-out needed.
      }
    }
  }
}

// SD TEST
void printDirectory(File dir, int numTabs) {
  while(true) {

    File entry =  dir.openNextFile();
    if (!entry) {
      // no more files
      //Serial.println("**nomorefiles**");
      break;
    }
    for (uint8_t i=0; i<numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs+1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

//
File root;
void setup() {
  // //DEBUG:boot testing
  // pinMode(13, OUTPUT);
  // digitalWrite(13, LOW); // LOW: OFF

  //serial monitor
  Serial.begin(115200);
  delay(50);

  //motor
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);

  //i2c
  //NOTE: But, don't use pull-up registers for esp32 boards. -> esp32 i2c function will drive bus pull-up on the fly!
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH); // for i2c pullup
  pinMode(22, OUTPUT);
  digitalWrite(22, HIGH); // for i2c pullup
  Wire.begin(i2c_addr);
  Wire.onReceive(receiveEvent);
  //Wire.onRequest(requestEvent);

#if (BOARD_SELECT == BOARD_TEENSY_35)
  //SD - AudioPlaySdWav @ teensy audio library needs SD.begin() first. don't forget/ignore!
  //+ let's additionally check contents of SD.
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("[sd] initialization failed!");
    return;
  }
  Serial.println("[sd] initialization done.");
  root = SD.open("/");
  printDirectory(root, 0);

  //audio
  AudioMemory(20);
  //NOTE:reduced volume
  mixer1.gain(0,0.7);
  mixer1.gain(1,0.7);
  mixer1.gain(2,0);
  mixer1.gain(3,0);
  mixer2.gain(0,0.7);
  mixer2.gain(1,0.7);
  mixer2.gain(2,0);
  mixer2.gain(3,0);

#elif (BOARD_SELECT == BOARD_TEENSY_32)
  //SPI
  SPI.setSCK(14);
  //SD - AudioPlaySdWav @ teensy audio library needs SD.begin() first. don't forget/ignore!
  //+ let's additionally check contents of SD.
  if (!SD.begin(10)) {
    Serial.println("[sd] initialization failed!");
    // return;
  }
  Serial.println("[sd] initialization done.");
  root = SD.open("/");
  printDirectory(root, 0);

  //audio
  AudioMemory(20);
  mixer1.gain(0,1);
  mixer1.gain(1,1);
  mixer1.gain(2,0);
  mixer1.gain(3,0);
#endif

  //let auto-poweroff speakers stay turned ON!
  sine1.frequency(IDLE_FREQ);

  //led
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // LOW: OFF

  //player task
  runner.addTask(sound_player_start_task);
  runner.addTask(sound_player_stop_task);

  //
  Serial.println("[setup] done.");

  // //DEBUG: simple looping play all.
  // song_now = 0;
}

void loop() {
  runner.execute();

  // //DEBUG: simple looping play all.
  // if (playSdWav1.isPlaying() == false) {
  //   song_now += 1;
  //   if (song_now > 6) {
  //     song_now = 1;
  //   }
  //   sound_player_start_task.restart();
  //   delay(200);
  // }

}
