#pragma once

//i2c address
#define I2C_ADDR 3

//i2c protocol
#define CMD_LENGTH 10
#define CMD_BUFF_LEN (CMD_LENGTH + 1)
char cmdstr[CMD_BUFF_LEN] = "P#SS-/-/-/";

// "P#SS-/-/-/" - P: P (play), SS: score #
// "S-/-/-/-/-" - S: S (stop)
