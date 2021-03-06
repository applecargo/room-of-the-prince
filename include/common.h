#pragma once

// member identity
#define ID_BAG           (0x1D00 + 0x10)
#define ID_BELL          (0x1D00 + 0x20)
#define ID_CONDUCTOR     (0x1D00 + 0x30)
#define ID_DISTANCE      (0x1D00 + 0x40)
// #define ID_DOOR          (0x1D00 + 0x50)
#define ID_FLOAT         (0x1D00 + 0x60)
#define ID_FUR           (0x1D00 + 0x70)
#define ID_GPS           (0x1D00 + 0x80)
#define ID_HANGER        (0x1D00 + 0x90)
#define ID_HARMONICA     (0x1D00 + 0xA0)
#define ID_HEATER        (0x1D00 + 0xB0)
#define ID_KEYHOLDER     (0x1D00 + 0xC0)
#define ID_LOOK_AT       (0x1D00 + 0xD0)
#define ID_MIRROR        (0x1D00 + 0xE0)
// #define ID_MOTION_SENSOR (0x1D00 + 0xF0)
#define ID_PEAK          (0x1E00 + 0x00)
#define ID_ROCKING       (0x1E00 + 0x10)
#define ID_THUNDER       (0x1E00 + 0x20)

// #define ID_TUNER         (0x1E00 + 0x30)
// #define ID_RELAY_CTRLER  (0x1E00 + 0x40)
// #define ID_POINT_MOTOR   (0x1E00 + 0x50)

//
#define ID_EVERYONE      (0x1F00 + 0x00)
//(choice)
#define IDENTITY         ID_FLOAT

#define NUM_OF_MEMBERS 16
int memberList[NUM_OF_MEMBERS] = {
  ID_BAG, //1
  ID_BELL, //2
  ID_CONDUCTOR, //3
  ID_DISTANCE, //4
  ID_FLOAT, //5
  ID_FUR, //6
  ID_GPS, //7
  ID_HANGER, //8
  ID_HARMONICA, //9
  ID_HEATER, //10
  ID_KEYHOLDER, //11
  ID_LOOK_AT, //12
  ID_MIRROR, //13
  ID_PEAK, //14
  ID_ROCKING, //15
  ID_THUNDER //16
};

//
#define LONELY_TO_DIE    (30000)

//
#include <Arduino.h>

//
#include <painlessMesh.h>
extern painlessMesh mesh;

//NOTE: disabling AP beacon for speaker accompanied devices!
#define NODE_TYPE_AP_STA    (0x40DE0001)
#define NODE_TYPE_STA_ONLY  (0x40DE0002)
#define NODE_TYPE           NODE_TYPE_AP_STA
#if (IDENTITY == ID_THUNDER || IDENTITY == ID_GPS)
#undef NODE_TYPE
#define NODE_TYPE           NODE_TYPE_STA_ONLY
#endif

// board
#define BOARD_NODEMCU_ESP12E (0xBD00 + 1)
#define BOARD_NODEMCU_ESP12N (0xBD00 + 2)
#define BOARD_NODEMCU_ESP32  (0xBD00 + 3)
//(choice)
#define BOARD_SELECT BOARD_NODEMCU_ESP12E

// mesh
#define MESH_SSID "room-of-the-prince"
#define MESH_PASSWORD "12*1000/128"
#define MESH_PORT 5555
#define MESH_CHANNEL 5
// #define MESH_ANCHOR

//
// LED status indication
//   operation modes
//     0 - booted. and running. no connection. scanning.
//     1 - + connected.
//   notifying patterns
//     0 - steady on
//     1 - slow blinking (syncronized)
//
#if (BOARD_SELECT == BOARD_NODEMCU_ESP12E)
#define LED_PIN 2 // nodemcuv2
#elif (BOARD_SELECT == BOARD_NODEMCU_ESP32)
#define LED_PIN 13 // featheresp32
#endif
#define LED_PERIOD (1000+(12*1000/128))
#define LED_ONTIME (12*1000/128)

// event handlers fo connection-related events
extern void gotMessageCallback(uint32_t from, String & msg); // REQUIRED
extern void gotChangedConnectionCallback();

// the system scheduler
extern Scheduler runner;

#include "words.h"
