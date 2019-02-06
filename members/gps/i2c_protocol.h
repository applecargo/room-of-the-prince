#pragma once

//i2c address
#define I2C_ADDR 3

//i2c protocol
#define CMD1_LENGTH 22
#define CMD1_BUFF_LEN (CMD1_LENGTH + 1)
char cmdstr1[CMD1_BUFF_LEN] = "Ff@YYYY.MM.DD.hh.mm.ss";

// (NOTE: i2c by default, 32 bytes is the max. so... we need 2 different messages..)

#define CMD2_LENGTH 23
#define CMD2_BUFF_LEN (CMD2_LENGTH + 1)
char cmdstr2[CMD2_BUFF_LEN] = "#XXX.XXXXXX&YYYY.YYYYYY";

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

//
// "#XXXXXXXXX&YYYYYYYYYY"
//

//   - XXX.XXXXXX: latitude (float),
//   - YYYY.YYYYYY: longitude (float)
