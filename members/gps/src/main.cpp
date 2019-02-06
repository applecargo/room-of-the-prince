#include <Arduino.h>

//rtc
#include <TimeLib.h>
static time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

void rtc_set(time_t t) {

  //shortly stop sync.
  //  --> https://github.com/PaulStoffregen/Time/blob/a8f9be7907fb1ab27a5962ec9eb0ff6570b8d5d8/Time.cpp#L261
  setSyncProvider(NULL);
  // NULL == 0 ? if this is true, then this will work for stoping sync. as expected.

  Teensy3Clock.set(t);

  //restart sync.
  setSyncProvider(getTeensy3Time);
}

//gps
#include <TinyGPS.h> // --> https://github.com/florind/TinyGPS (one that's included in 'framework-arduinoteensy')
static TinyGPS gps;

//timezone
#include <Timezone.h>
static TimeChangeRule mySTD = {"KST", First, Sun, Jan, 0, +540};     //Standard time : KST = UTC + 9 hours
static Timezone myTZ(mySTD, mySTD);    //KST doesn't have DST. (hence, same TCR, twice.)
static TimeChangeRule *tcr;    //pointer to the time change rule, use to get TZ abbrev
static time_t local;

//location
static float latitude = 0.0;
// static float latitude = -82.105820;
static char lat = 'X';
static float longitude = 0.0;
// static float longitude = -145.295872;
static char lon = 'Y';
static float course = 0;
static float speed = 0;
static String cardinal = "";
static int nsat = 0;
static bool has_fix = false;

//fix age (validity)
static unsigned long fix_age_datetime; //ms
static unsigned long fix_age_position; //ms

//timer1
#include <TimerOne.h>
// NMEA string reader - 1ms timer interrupt
void readGPS(void) {
  //consume all characters in Serial1 buffer.
  char c = ' ';
  while (Serial1.available()) {
    c = Serial1.read();
    gps.encode(c);
    // Serial.print(c);
  }
}

//task
#include <TaskScheduler.h>
Scheduler runner;
//
void time_location_update() {

  // Serial.println("time_location_update");

  //a "got a new sentence" flag
  static bool newNMEA = false;
  static unsigned short sentences_prev = 0;

  //NOTE: following are 'static' to minimize memory allocation time.
  //      --> all of these must be updated before used.

  //statistics (reliability)
  static unsigned long chars;
  static unsigned short sentences;
  static unsigned short failed_checksum;
  // //fix age (validity)
  // static unsigned long fix_age_datetime; //ms
  // static unsigned long fix_age_position; //ms
  //position
  static float flat, flon;
  //time & date
  // static unsigned long time, date;
  static int year;
  static byte month, day, hour, minute, second, hundredth;
  //speed, course (estimation?)
  static float fspeed, fcourse;
  static String cardinal;

  //shortly stop getting more characters
  // Timer1.detachInterrupt();

  //
  has_fix = false;

  //
  gps.stats(&chars, &sentences, &failed_checksum);
  // Serial.print("chars: ");
  // Serial.println(chars);
  // Serial.print("sentences: ");
  // Serial.println(sentences);
  // Serial.print("failed_checksum: ");
  // Serial.println(failed_checksum);

  newNMEA = false;
  if (sentences > sentences_prev) {

    ////parse it

    //position
    gps.f_get_position(&flat, &flon, &fix_age_position); //deg

    //time & date
    // gps.get_datetime(&date, &time, &fix_age);
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredth, &fix_age_datetime);

    //NOTE: check if above 2 'fix_age' have different context or not? but, how to?

    //speed, course, cardinal
    fspeed = gps.f_speed_kmph(); //km/h
    fcourse = gps.f_course(); //deg
    cardinal = String(gps.cardinal(fcourse)); //c_str

    //got a new sentence & we've parsed it!
    newNMEA = true;
  }
  sentences_prev = sentences;

  //re-enable receiving chars
  // Timer1.attachInterrupt(readGPS);

  if (newNMEA) {
    ////
    // update system time & rtc time. if there's a fix.
    ////

    if (fix_age_datetime < 5000 && fix_age_datetime != TinyGPS::GPS_INVALID_AGE) {
      // set systemtime
      setTime(hour, minute, second, day, month, year);

      // set rtc (by systemtime)
      rtc_set(now());
    }

    ////
    // update location if there's a fix.
    ////

    if (fix_age_position < 5000 && fix_age_position != TinyGPS::GPS_INVALID_AGE) {
      if (latitude >= 0) {
        lat = 'N';
        latitude = flat;
      } else {
        lat = 'S';
        latitude = flat * -1;
      }
      if (longitude >= 0) {
        lon = 'E';
        longitude = flon;
      } else {
        lon = 'W';
        longitude = flon * -1;
      }

      //course, cardinal
      // cardinal = cardinal; //???
      course = fcourse;
      speed = fspeed;

      //DEV. mode : n of sat.
      nsat = (int)gps.satellites();

      //a flag for availability of the pos. fix.
      has_fix = true;
    }
  }

  ////
  // 2) localize the time according to the TZ.
  ////

  // timezone
  local = myTZ.toLocal(now(), &tcr); // tcr : to get TZ abbrev of 'now'

  // Serial.print("has_fix: ");
  // Serial.println(has_fix);
  // Serial.print("fix_age_position: ");
  // Serial.println(fix_age_position);
}
Task time_location_update_task(1000, TASK_FOREVER, time_location_update);

//i2c
//pin connection
//nodemcu D1 <-> arduino nano A5 (SCL)
//nodemcu D2 <-> arduino nano A4 (SDA)
#include <Wire.h>
#include "../i2c_protocol.h"
static byte cmd_type = 1;
void receiveEvent(int numBytes) {
  // Serial.println("[i2c] on receive!");
  while(Wire.available()) {
    cmd_type = Wire.read();
  }
  // Serial.print("[i2c] cmd_type req. : ");
  // Serial.println(cmd_type);
  // Serial.print("numBytes: ");
  // Serial.println(numBytes);

  //NOTE: there's a weird bug now..
  //example output :
  // 1
  // [i2c] cmd_type req. : 1
  // numBytes: 1
  //
  // F0@2019.02.06.21.44.11
  // ␁1
  // [i2c] cmd_type req. : 1
  // numBytes: 2
  //
  // 1
  // [i2c] cmd_type req. : 1
  // numBytes: 1
  //
  // F0@2019.02.06.21.44.12
  // ␁␁1
  // [i2c] cmd_type req. : 1
  // numBytes: 3
  //
  // 1
  // [i2c] cmd_type req. : 1
  // numBytes: 1

  // so, master(nodemcuv2: esp8266) is apparently sending only 1 byte..
  // but, teensy i2c reads..more bytes.. and also get triggered twice!
  // this might be a bug of 'master' or 'slave'..
  // well. this doesn't affect, desired operation. so let's ignore it now.
  // but gives me un-ease feeling.... what is this.
  //
}
void requestEvent() {
  if (cmd_type == 1) {
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

    sprintf(cmdstr1, "F%d@%04d.%02d.%02d.%02d.%02d.%02d",
            (has_fix ? 1 : 0),
            year(local),
            month(local),
            day(local),
            hour(local),
            minute(local),
            second(local));
    Wire.write(cmdstr1);
    Serial.println(cmdstr1);
  } else if (cmd_type == 2) {
    //
    // "#XXXXXXXXX&YYYYYYYYYY"
    //

    //   - XXX.XXXXXX: latitude (float),
    //   - YYYY.YYYYYY: longitude (float)

    sprintf(cmdstr2, "#%010.6f&%011.6f",
            latitude,
            longitude);
    Wire.write(cmdstr2);
    Serial.println(cmdstr2);
  }
}

//
void setup() {
  //serial monitor
  Serial.begin(9600);

  //i2c
  Wire.begin(I2C_ADDR);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  //serial1 - GPS comm.
  Serial1.begin(9600);

  //timer1 - readGPS() - read in 1 char.
  Timer1.initialize(1000); //1ms
  Timer1.attachInterrupt(readGPS);

  //NOTE: somehow.. using TinyGPS, i see 1 to 5 sec. delay in time sync.
  //  --> some says.. NMEA string processing/parsing is not on-time.
  //      ( https://forum.arduino.cc/index.php?topic=418221.msg2881294#msg2881294 )
  //  --> for now, i have no idea. how to improve this. want < 1sec.
  //  --> using interrupt-driven way of Serial1 processing is being recommanded.
  //  --> researching into Adafruit GPS codebase might give a hint.
  //  --> but, for now, i have no idea. let's move on, skipping this.

  //EDIT: adafruit gps library also has this 5 sec delay.
  //  --> so, it is not a prob. of library. the GPS time itself is 5 sec off?
  //  --> they say it is non-sense. such delay for GPS to calculate position.
  //  --> but then this could be a comm. delay from when the position calculated and then transmitted. ?

  //EDIT: WOW.
  //  --> my android phone has time mismatch to the atomic time!
  //  --> iphone time is well match to the device's GPS synced time!
  //  --> http://leapsecond.com/java/gpsclock.htm
  //  --> https://www.zdnet.com/article/the-time-displayed-on-most-android-phones-is-wrong/
  //  --> https://android.gadgethacks.com/how-to/set-your-android-atomic-time-for-perfectly-synced-clock-0170500/

  //rtc
  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }

  //tasks
  runner.init();
  //
  runner.addTask(time_location_update_task);
  time_location_update_task.enable();
}

void loop() {
  runner.execute();
}
