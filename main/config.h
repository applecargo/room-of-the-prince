#pragma once

//// boards
#define BOARD_DOIT          (0xBD01)
#define BOARD_FEATHER32     (0xBD02)
//// -->
#define BOARD               (BOARD_DOIT)

//// identities
#define ID_MOTION_SENSOR    (0x01)
#define ID_RELAY_CTRL       (0x02)
#define ID_POINTER_CTRL     (0x03)
//// -->
#define ID                  ID_MOTION_SENSOR

//// shared (C <-> C++ : main.c <-> main_arduino.cpp)
extern const uint8_t my_mac[6];
extern mesh_addr_t base_addr;
extern const char *MESH_TAG;
extern const uint8_t MESH_ID[6];

#ifdef __cplusplus
extern "C" {
#endif
void arduino_start();
void mesh_event_handler(mesh_event_t event);
#ifdef __cplusplus
}
#endif
