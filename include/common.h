#pragma once

//
#include <Arduino.h>

//
#include <painlessMesh.h>
extern painlessMesh mesh;

// member identity
#define ID_MOTION_SENSOR (0x1D00 + 1)
#define ID_RELAY_CTRLER  (0x1D00 + 2)
#define ID_POINT_MOTOR   (0x1D00 + 3)
//(choice)
#define IDENTITY ID_POINT_MOTOR

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
#if (BOARD_SELECT==BOARD_NODEMCU_ESP12E)
#define LED_PIN 2 // nodemcuv2
#elif (BOARD_SELECT==BOARD_NODEMCU_ESP32)
#define LED_PIN 13 // featheresp32
#endif
#define LED_PERIOD (1000+(12*1000/128))
#define LED_ONTIME (12*1000/128)

// event handlers fo connection-related events
extern void gotMessageCallback(uint32_t from, String & msg); // REQUIRED
extern void gotChangedConnectionCallback();

// the system scheduler
extern Scheduler runner;
