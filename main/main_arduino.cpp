#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_mesh_internal.h"
#include "nvs_flash.h"

#include "Arduino.h"
#include "config.h"

// mesh
const char *MESH_TAG = "room_mesh";
const uint8_t MESH_ID[6] = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77}; // mesh ID should be same among peers
#define RX_BUF_SIZE (512)
#define TX_BUF_SIZE (512)
static uint8_t rx_buf[RX_BUF_SIZE] = { 0, };
static uint8_t tx_buf[TX_BUF_SIZE] = { 0, };
static bool is_running = true;

// vocabulary list
char voca[10][256] = {0, };

//// apply board & id configurations
#if (BOARD == BOARD_DOIT)
#define LED_BUILTIN 2
#elif (BOARD == BOARD_FEATHER32)
#define LED_BUILTIN 13
#endif
#if (ID == ID_MOTION_SENSOR)
const uint8_t my_mac[6] = {0x24, 0x0a, 0xc4, 0x0f, 0x8d, 0xb4};
#include "arduino_apps/motion.cpp"
#elif (ID == ID_RELAY_CTRL)
const uint8_t my_mac[6] = {0x24, 0x0a, 0xc4, 0x10, 0x50, 0x98};
#include "arduino_apps/relay.cpp"
#elif (ID == ID_POINTER_CTRL)
const uint8_t my_mac[6] = {0x24, 0x0a, 0xc4, 0x0e, 0x8b, 0x10};
#include "arduino_apps/pointer.cpp"
#endif

//// arduino
static bool is_running_arduino = true;
void arduino_main(void *arg) {
  is_running_arduino = true;
  while (is_running_arduino) {
    loop();
  }
  vTaskDelete(NULL);
}
extern "C" void arduino_start() {
  setup();
  xTaskCreate(arduino_main, "arduino_mainloop", 3072, NULL, 5, NULL);
  /* BaseType_t xTaskCreate(TaskFunction_t pvTaskCode,
   *                        const char * const pcName,
   *                        unsigned short usStackDepth,
   *                        void *pvParameters,
   *                        UBaseType_t uxPriority,
   *                        TaskHandle_t *pxCreatedTask
   *                        );
   */
}

//MESH TX
void esp_mesh_p2p_tx_main(void *arg)
{
  //
  int count = 0;
  int idx = 0;

  //
  mesh_addr_t route_table[CONFIG_MESH_ROUTE_TABLE_SIZE];
  int route_table_size = 0;

  //
  mesh_data_t data;
  data.data = tx_buf;
  data.size = sizeof(tx_buf);
  data.proto = MESH_PROTO_BIN;
  // data.tos = MESH_TOS_DEF;

  //
  is_running = true;
  while (is_running) {

    //loop counter
    count++;

    //get peer list
    esp_mesh_get_routing_table((mesh_addr_t *) &route_table, CONFIG_MESH_ROUTE_TABLE_SIZE * 6, &route_table_size);

    //fill the content
    idx = count % 5;
    memcpy(tx_buf, (uint8_t *)voca[idx], 256);

    //send to all == broadcast (including the sender)
    for (int i = 0; i < route_table_size; i++) {
      esp_err_t err = esp_mesh_send(&route_table[i], &data, MESH_DATA_P2P, NULL, 0);
      ESP_LOGI(MESH_TAG, "SENT-TO:%x:%x:%x:%x:%x:%x",
               route_table[i].addr[0],
               route_table[i].addr[1],
               route_table[i].addr[2],
               route_table[i].addr[3],
               route_table[i].addr[4],
               route_table[i].addr[5]);
      if (err) {
        ESP_LOGE(MESH_TAG, "WHAT?");
      }
    }

    //delay 100 ms
    vTaskDelay(100 / portTICK_RATE_MS);
  }

  //
  vTaskDelete(NULL);
}

//MESH RX
void esp_mesh_p2p_rx_main(void *arg)
{
  //
  mesh_addr_t from;
  mesh_data_t data;
  int flag = 0;
  data.data = rx_buf;
  data.size = RX_BUF_SIZE;

  //
  is_running = true;
  while (is_running) {
    data.size = RX_BUF_SIZE;
    esp_err_t err = esp_mesh_recv(&from, &data, portMAX_DELAY, &flag, NULL, 0);
    if (err != ESP_OK || !data.size) {
      ESP_LOGE(MESH_TAG, "err:0x%x, size:%d", err, data.size);
      continue;
    }
    ESP_LOGI(MESH_TAG, "STA:%x:%x:%x:%x:%x:%x",
             my_mac_sta[0],
             my_mac_sta[1],
             my_mac_sta[2],
             my_mac_sta[3],
             my_mac_sta[4],
             my_mac_sta[5]);
    // ESP_LOGI(MESH_TAG, "AP:%x:%x:%x:%x:%x:%x",
    //          my_mac_ap[0],
    //          my_mac_ap[1],
    //          my_mac_ap[2],
    //          my_mac_ap[3],
    //          my_mac_ap[4],
    //          my_mac_ap[5]);
    //from.addr is equal to the sender's STA mac addr. (not AP addr..)
    //but this might differs if parent-child relationship opposite way?
    //so, this means that receiver was actually AP and the sender was STA.. so STA mac. ?
    //but.. from the child's view point.. received packets are being reported as a sent of STA of the parent. not AP.
    //connected-to is apparently to AP's mac.. but received packets are NOT from the AP's mac addr.. but from STA's mac addr.
    //so.. then we can safely assume that anyway STA mac is being used as the unique ID of the node???
    // maybe??? let's accept that for now, and go on.
    ESP_LOGI(MESH_TAG, "RX:%x:%x:%x:%x:%x:%x, %s", from.addr[0], from.addr[1], from.addr[2], from.addr[3], from.addr[4], from.addr[5], data.data);

    //delay 10 ms
    vTaskDelay(10 / portTICK_RATE_MS);
  }

  //
  vTaskDelete(NULL);
}

//MESH COMM. STARTER
esp_err_t esp_mesh_comm_p2p_start(void)
{
  static bool is_comm_p2p_started = false;
  if (!is_comm_p2p_started) {
    is_comm_p2p_started = true;
    xTaskCreate(esp_mesh_p2p_tx_main, "MPTX", 3072, NULL, 5, NULL);
    xTaskCreate(esp_mesh_p2p_rx_main, "MPRX", 3072, NULL, 5, NULL);
  }
  return ESP_OK;
}

//MESH EVT. HANDLER (this func. will be called from the framework!) --> C linkage.
extern "C" void mesh_event_handler(mesh_event_t event) {
  switch (event.id) {
  case MESH_EVENT_PARENT_CONNECTED:
    esp_mesh_comm_p2p_start();
    break;
  default:
    ;
  }
}
