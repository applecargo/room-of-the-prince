#include <Arduino.h>

//teensy audio
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine sine1;            //xy=297,249
AudioAmplifier amp1;                     //xy=476,249
AudioOutputAnalogStereo dacs1;           //xy=663,247
AudioConnection patchCord1(sine1, amp1);
AudioConnection patchCord2(amp1, 0, dacs1, 0);
AudioConnection patchCord3(amp1, 0, dacs1, 1);
// GUItool: end automatically generated code

//i2c
#include <Wire.h>
#include "../i2c_protocol.h"
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
    //protocol: F0000A0000
    String str_freq = msg.substring(1,5); // 1234
    String str_gain = msg.substring(6,10); // 6789
    float sine_freq = str_freq.toFloat();
    float amp_gain = str_gain.toFloat() * 0.001;

    // Serial.print("sine_freq:");
    // Serial.println(sine_freq);
    // Serial.print("sine_amp:");
    // Serial.println(sine_amp);

    //process commands
    sine1.frequency(sine_freq);
    amp1.gain(amp_gain);
  }
}

void setup() {
  //
  Serial.begin(9600);

  //i2c
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receiveEvent);

  //
  AudioMemory(10);

  //
  sine1.frequency(0);
  sine1.amplitude(1.0);

  //
  amp1.gain(1.0);
}

void loop() {

}
