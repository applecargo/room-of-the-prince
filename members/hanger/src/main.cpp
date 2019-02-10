//wind sense
float getWindSpeed();

//teensy audio
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

//teensy 3.5 with SD card
#define SDCARD_CS_PIN    BUILTIN_SDCARD
#define SDCARD_MOSI_PIN  11  // not actually used
#define SDCARD_SCK_PIN   13  // not actually used

// GUItool: begin automatically generated code
AudioPlaySdWav playSdWav1;               //xy=224,265
AudioSynthWaveformSine sine1;            //xy=236,361
AudioMixer4 mixer2;                      //xy=497,328
AudioMixer4 mixer1;                      //xy=499,245
AudioAmplifier amp1;                     //xy=633,245
AudioAmplifier amp2;                     //xy=634,328
AudioOutputAnalogStereo dacs1;           //xy=788,284
AudioConnection patchCord1(playSdWav1, 0, mixer1, 0);
AudioConnection patchCord2(playSdWav1, 1, mixer2, 0);
AudioConnection patchCord3(sine1, 0, mixer1, 1);
AudioConnection patchCord4(sine1, 0, mixer2, 1);
AudioConnection patchCord5(mixer2, amp2);
AudioConnection patchCord6(mixer1, amp1);
AudioConnection patchCord7(amp1, 0, dacs1, 0);
AudioConnection patchCord8(amp2, 0, dacs1, 1);
// GUItool: end automatically generated code

//task
#include <TaskScheduler.h>
Scheduler runner;
//song #
int song_now = 0; //0~99
//
extern Task wind_sense_task;
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
  //to wait a bit for updating isPlaying()
  delay(10);
}
void sound_player_stop() {
  //stop the player.
  if (playSdWav1.isPlaying() == true) {
    playSdWav1.stop();
  }
}
void sound_player_check() {
  if (playSdWav1.isPlaying() == false) {
    //mark the indicator : LOW: OFF
    digitalWrite(13, LOW);
    //wind sense task sound & processing ON.
    sine1.amplitude(0.3);
    wind_sense_task.restart();
  }
  else {
    //wind sense task sound & processing OFF.
    sine1.amplitude(0);
    wind_sense_task.disable();
  }
}
//
Task sound_player_start_task(0, TASK_ONCE, sound_player_start);
Task sound_player_stop_task(0, TASK_ONCE, sound_player_stop);
Task sound_player_check_task(0, TASK_FOREVER, sound_player_check, &runner, true);

//wind sensor task
void wind_sense()
{
  // get wind data...
  float windspeed = getWindSpeed();
  Serial.print("windspeed => ");
  Serial.println(windspeed);
  //
  // sine1.amplitude(0.3);
  sine1.frequency(windspeed*10);
}
Task wind_sense_task(20, TASK_FOREVER, wind_sense);

//i2c
#include <Wire.h>
#include "../i2c_protocol.h"
void receiveEvent(int numBytes) {
  //numBytes : how many bytes received(==available)

  // Serial.println("[i2c] on receive!");
  int nb = Wire.readBytes(cmdstr, CMD_LENGTH);
  Serial.print("[i2c] cmdstr : ");
  Serial.println(cmdstr);

  if (CMD_LENGTH == nb) { // receive cmdstr.

    //convert to String
    String msg = String(cmdstr);

    //parse command string.
    char first = msg.charAt(0);
    if (first == 'P') {
      //
      // "P#SSS@AAAA" - P: P (play), SSS: song #, A: amp. (x 1000)
      //
      String str_song = msg.substring(2,5); // 234
      String str_gain = msg.substring(6,10); // 6789
      song_now = str_song.toInt();
      float amp_gain = str_gain.toFloat() * 0.001;
      //
      amp1.gain(amp_gain);
      amp2.gain(amp_gain);
      sound_player_start_task.restart();
      //
    } else {
      //
      // "SXXXX@AAAA" - S: S (stop), A: amp. (x 1000)
      //
      String str_gain = msg.substring(6,10); // 6789
      float amp_gain = str_gain.toFloat() * 0.001;
      sound_player_stop_task.restart();
      amp1.gain(amp_gain);
      amp2.gain(amp_gain);
      //
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

  //serial monitor
  Serial.begin(9600);
  delay(50);

  //i2c
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receiveEvent);
  //Wire.onRequest(requestEvent);

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
  mixer1.gain(0,1.0);
  mixer1.gain(1,1.0);
  mixer1.gain(2,0);
  mixer1.gain(3,0);
  mixer2.gain(0,1.0);
  mixer2.gain(1,1.0);
  mixer2.gain(2,0);
  mixer2.gain(3,0);
  amp1.gain(1.0);
  amp2.gain(1.0);

  //
  sine1.frequency(0);

  //led
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // LOW: OFF

  //player task
  runner.addTask(sound_player_start_task);
  runner.addTask(sound_player_stop_task);

  //wind sense task
  runner.addTask(wind_sense_task);
  wind_sense_task.enable();

  //
  Serial.println("[setup] done.");
}

void loop() {
  runner.execute();
}

//wind speed measurement
#define analogPinForRV    1
#define analogPinForTMP   0
float getWindSpeed() {
  //parameter
  const float zeroWindAdjustment =  .2;
  //
  static int TMP_Therm_ADunits = 0;
  static float RV_Wind_ADunits = 0;
  static float RV_Wind_Volts = 0;
  static int TempCtimes100 = 0;
  static float zeroWind_ADunits = 0;
  static float zeroWind_volts = 0;
  static float WindSpeed_MPH = 0;

  TMP_Therm_ADunits = analogRead(analogPinForTMP);
  RV_Wind_ADunits = analogRead(analogPinForRV);
  RV_Wind_Volts = (RV_Wind_ADunits *  0.0048828125);
  TempCtimes100 = (0.005 * ((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits)) - (16.862 * (float)TMP_Therm_ADunits) + 9075.4;
  zeroWind_ADunits = -0.0006 * ((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits) + 1.0727 * (float)TMP_Therm_ADunits + 47.172; //  13.0C  553  482.39
  zeroWind_volts = (zeroWind_ADunits * 0.0048828125) - zeroWindAdjustment;

  return pow(((RV_Wind_Volts - zeroWind_volts) / .2300), 2.7265);
}
