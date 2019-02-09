// i2c
#include <Wire.h>
#include "gps/i2c_protocol.h"

// my tasks
extern Task gps_task;
extern Task saying_greeting;

// room protocol
static int message = 0;
static char msg_cstr[MSG_LENGTH_MAX] = {0, };
extern Task reaction_task;
void gotChangedConnectionCallback() { // REQUIRED
}
void gotMessageCallback(uint32_t from, String & msg) { // REQUIRED
  Serial.println(msg);
  // is it for me?
  int receipent = msg.substring(1, 7).toInt();
  if (receipent == IDENTITY) {
    // what it says?
    message = msg.substring(8, 12).toInt();
    // i ve heard. reaction.
    reaction_task.restart();
    // so, what to do, then?
    switch (message)
    {
    default:
      ;
    }
  }
}

// some reaction for received msg.
void reaction() {
  static int mask = 0x8000;
  static int count = 0;
  if (reaction_task.isFirstIteration()) {
    mask = 0x8000;
    count = 0;
  }
  if ((message & mask) == 0) {
    //what to do?
  }
  else {
    //what to do?
  }
  mask = mask >> 1;
  count++;
}
Task reaction_task(10, 16, &reaction);

// saying hello
void greeting() {
  static String msg = "";
  sprintf(msg_cstr, "[%06d:%03d]", ID_EVERYONE, GPS_WORD_HELLO); //"1000 1010 0010 1111 1010 1010 IoIo."
  msg = String(msg_cstr);
  mesh.sendBroadcast(msg);
}
Task saying_greeting(10000, TASK_FOREVER, &greeting);

// gps
void gps_reader() {

  Serial.println("gps_reader");

  //
  bool has_fix = false;
  float latitude = 0.0;
  float longitude = 0.0;

  //fetch cmdstr1
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(1);
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, CMD1_LENGTH); //request cmdstr1.

  if (CMD1_LENGTH == Wire.readBytes(cmdstr1, CMD1_LENGTH)) { // receive cmdstr1.
    //convert to String
    String msg = String(cmdstr1);

    //monitoring
    Serial.println(msg);

    //
    // "Ff@YYYY.MM.DD.hh.mm.ss"
    //

    //   - F: fix (yes/no - 1/0),
    //   - YYYY: year,
    //   - MM: month,
    //   - DD: day,
    //   - hh: hour,
    //   - mm: minute,
    //   - ss: seconds, (millisec??)

    // date & time
    // "Ff@YYYY.MM.DD.hh.mm.ss"
    //     3456789012345678901
    String datetime = msg.substring(3, 22); // 3 ~ 21

    if (msg.charAt(1) == '1') { // got fix.

      //
      has_fix = true;

      //
      Wire.beginTransmission(I2C_ADDR);
      Wire.write(2);
      Wire.endTransmission();
      Wire.requestFrom(I2C_ADDR, CMD2_LENGTH); //request cmdstr2.

      if (CMD2_LENGTH == Wire.readBytes(cmdstr2, CMD2_LENGTH)) { // receive cmdstr2.
        //convert to String
        String msg = String(cmdstr2);

        //
        // "#XXXXXXXXX&YYYYYYYYYY"
        //

        //   - XXX.XXXXXX: latitude (float),
        //   - YYYY.YYYYYY: longitude (float)

        // "#XXX.XXXXXX&YYYY.YYYYYY"
        //   1234567890
        Serial.println(msg.substring(1, 11));
        latitude = (msg.substring(1, 11)).toFloat(); // 1 ~ 10

        // "#XXX.XXXXXX&YYYY.YYYYYY"
        //          (11)23456789012
        Serial.println(msg.substring(12, 23));
        longitude = (msg.substring(12, 23)).toFloat(); // 12 ~ 22
      }
    }

    // regular report about the planet
    if (has_fix) {
      sprintf(msg_cstr, "[%06d:%03d] To everyone: Now, our planet is at. (%f, %f) on time of %s", ID_EVERYONE, GPS_WORD_PLANETARY_ANNOUNCEMENT, latitude, longitude, datetime.c_str());
    } else {
      sprintf(msg_cstr, "[%06d:%03d] To everyone: Now, our planet is floating in the universe. on time of %s", ID_EVERYONE, GPS_WORD_PLANETARY_ANNOUNCEMENT, datetime.c_str());
    }
    msg = String(msg_cstr);
    mesh.sendBroadcast(msg);
    //
    Serial.println(msg_cstr);
  }
}
Task gps_reader_task(20000, TASK_FOREVER, &gps_reader);

//setup_member
void setup_member() {
  //i2c master
  Wire.begin();

  //tasks
  runner.addTask(saying_greeting);
  saying_greeting.enable();
  runner.addTask(gps_reader_task);
  gps_reader_task.enable();
  runner.addTask(reaction_task);
}
