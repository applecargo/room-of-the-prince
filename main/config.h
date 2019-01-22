#pragma once

//// boards
#define BOARD_DOIT        (0xBD01)
#define BOARD_FEATHER32   (0xBD02)
//// -->
#define BOARD             (BOARD_DOIT)

//// identities
#define ID_MOTION_SENSOR  (0x1D01)
#define ID_RELAY_CTRL     (0x1D02)
#define ID_POINTER_CTRL   (0x1D03)
//// -->
#define ID                (ID_POINTER_CTRL)

//// shared
extern const char *MESH_TAG;
extern const uint8_t MESH_ID[6];
extern const uint8_t my_mac[6];
extern uint8_t my_mac_sta[6];
extern uint8_t my_mac_ap[6];

//// shared between C & C++
#ifdef __cplusplus
extern "C" {
#endif
void arduino_start();
void mesh_event_handler(mesh_event_t event);
#ifdef __cplusplus
}
#endif
