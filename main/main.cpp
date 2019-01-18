#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_mesh.h"
#include "esp_mesh_internal.h"
#include "nvs_flash.h"
#include <string>
#include <vector>

/* mesh */
static const char *MESH_TAG = "mesh_main";
static const uint8_t MESH_ID[6] = { 0x77, 0x77, 0x77, 0x77, 0x77, 0x77}; /* mac addr. */
#define RX_SIZE (1500)
#define TX_SIZE (1460)
static uint8_t tx_buf[TX_SIZE] = { 0, };
static uint8_t rx_buf[RX_SIZE] = { 0, };
static bool is_running = true;

/* identity */
#define MATE_MOTION_SENSOR 0x01
#define MATE_RELAY_CONTROLLER 0x02
int mate_id = MATE_RELAY_CONTROLLER;

std::string hello = "hello.";
std::vector<std::string> vec_str;

/* vocabularies */
char voca[10][256] = {0, };
void mate_motion_sensor_vocabulary()
{
  strcpy(voca[0], "0Hello, I'm a motion detector, called Meisenstroofpen.");
  strcpy(voca[1], "1Hey! There's something moving!!!");
  strcpy(voca[2], "2Well.. nothing is moving. How come?");
  strcpy(voca[3], "3Seriously, I want to know what was the thing right now? any idea?");
  strcpy(voca[4], "4Could you be more stay still for me? I cannot concentrate!");
}
void mate_motion_sensor_react()
{
  ;
}
void mate_relay_controller_vocabulary()
{
  strcpy(voca[0], "0Yo, I'm a relay controller, called Diego Larambla.");
  strcpy(voca[1], "1What! You mean there's a person?");
  strcpy(voca[2], "2Tick, tack, toe--.");
  strcpy(voca[3], "3I don't know either, amigo.");
  strcpy(voca[4], "4Ya well, I do sincerely apologize, amigo.");
}
void mate_relay_controller_react()
{
  ;
}

//file: main.cpp
#include "Arduino.h"
static const uint8_t LED_BUILTIN = 2; //doit

/* mesh comm. */
void esp_mesh_p2p_tx_main(void *arg)
{
  int i;
  esp_err_t err;
  int count = 0;
  int idx = 0;
  mesh_addr_t route_table[CONFIG_MESH_ROUTE_TABLE_SIZE];
  int route_table_size = 0;
  mesh_data_t data;
  data.data = tx_buf;
  data.size = sizeof(tx_buf);
  data.proto = MESH_PROTO_BIN;
// #ifdef MESH_P2P_TOS_OFF
//   data.tos = MESH_TOS_DEF;
// #endif /* MESH_P2P_TOS_OFF */
//
  is_running = true;
  while (is_running) {
    count++;
    idx = count % 5;
    esp_mesh_get_routing_table((mesh_addr_t *) &route_table, CONFIG_MESH_ROUTE_TABLE_SIZE * 6, &route_table_size);
    memcpy(tx_buf, (uint8_t *)voca[idx], 256);
    for (i = 0; i < route_table_size; i++) {
      err = esp_mesh_send(&route_table[i], &data, MESH_DATA_P2P, NULL, 0);
      if (err) {
        ESP_LOGE(MESH_TAG, "WHAT?");
      }
    }
    //delay 100 ms
    vTaskDelay(100 / portTICK_RATE_MS);
  }
  vTaskDelete(NULL);
}

void esp_mesh_p2p_rx_main(void *arg)
{
  // int recv_count = 0;
  esp_err_t err;
  mesh_addr_t from;
  // int send_count = 0;
  mesh_data_t data;
  int flag = 0;
  data.data = rx_buf;
  data.size = RX_SIZE;

  is_running = true;
  while (is_running) {
    data.size = RX_SIZE;
    err = esp_mesh_recv(&from, &data, portMAX_DELAY, &flag, NULL, 0);
    if (err != ESP_OK || !data.size) {
      ESP_LOGE(MESH_TAG, "err:0x%x, size:%d", err, data.size);
      continue;
    }
    ESP_LOGI(MESH_TAG, "RX:%s", data.data);
    //delay 10 ms
    vTaskDelay(10 / portTICK_RATE_MS);
  }
  vTaskDelete(NULL);
}

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

/* mesh_event_handler --> sth. mandatory ! */
void mesh_event_handler(mesh_event_t event) {
  switch (event.id) {
  case MESH_EVENT_PARENT_CONNECTED:
    esp_mesh_comm_p2p_start();
    break;
  default:
    ;
  }
}

void setup(){
  /* nvs flash */
  ESP_ERROR_CHECK(nvs_flash_init());

  /* tcpip initialization */
  tcpip_adapter_init();
  /* for mesh
   * stop DHCP server on softAP interface by default
   * stop DHCP client on station interface by default
   * */
  ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
  ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA));

  /* wifi initialization */
  ESP_ERROR_CHECK(esp_event_loop_init(NULL, NULL));
  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&config));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
  ESP_ERROR_CHECK(esp_wifi_start());

  /* mesh initialization */
  ESP_ERROR_CHECK(esp_mesh_init());
  ESP_ERROR_CHECK(esp_mesh_set_max_layer(CONFIG_MESH_MAX_LAYER));
  ESP_ERROR_CHECK(esp_mesh_set_vote_percentage(1));
  ESP_ERROR_CHECK(esp_mesh_set_ap_assoc_expire(10));
  #ifdef MESH_FIX_ROOT
  ESP_ERROR_CHECK(esp_mesh_fix_root(1));
  #endif
  mesh_cfg_t cfg;
  cfg.crypto_funcs = &g_wifi_default_mesh_crypto_funcs;

  /* mesh ID */
  memcpy((uint8_t *) &cfg.mesh_id, MESH_ID, 6);

  /* mesh event callback */
  cfg.event_cb = &mesh_event_handler;

  /* router */
  cfg.channel = CONFIG_MESH_CHANNEL;
  cfg.router.ssid_len = strlen(CONFIG_MESH_ROUTER_SSID);
  memcpy((uint8_t *) &cfg.router.ssid, CONFIG_MESH_ROUTER_SSID, cfg.router.ssid_len);
  memcpy((uint8_t *) &cfg.router.password, CONFIG_MESH_ROUTER_PASSWD, strlen(CONFIG_MESH_ROUTER_PASSWD));

  /* mesh softAP */
  ESP_ERROR_CHECK(esp_mesh_set_ap_authmode((wifi_auth_mode_t)CONFIG_MESH_AP_AUTHMODE));
  cfg.mesh_ap.max_connection = CONFIG_MESH_AP_CONNECTIONS;
  memcpy((uint8_t *) &cfg.mesh_ap.password, CONFIG_MESH_AP_PASSWD, strlen(CONFIG_MESH_AP_PASSWD));
  ESP_ERROR_CHECK(esp_mesh_set_config(&cfg));

  /* mesh start */
  ESP_ERROR_CHECK(esp_mesh_start());
  ESP_LOGI(MESH_TAG, "mesh starts successfully, heap:%d, %s\n",  esp_get_free_heap_size(), esp_mesh_is_root_fixed() ? "root fixed" : "root not fixed");

  // arduino start >>>

  // ESP_LOGI(MESH_TAG, "setup()");
  pinMode(LED_BUILTIN, OUTPUT);

  //
  if (mate_id == MATE_MOTION_SENSOR) {
    mate_motion_sensor_vocabulary();
  } else if (mate_id == MATE_RELAY_CONTROLLER) {
    mate_relay_controller_vocabulary();
  }

}

void loop(){
  // ESP_LOGI(MESH_TAG, "loop()");

  // //simple blink test-code.
  // digitalWrite(LED_BUILTIN, HIGH);
  // delay(100);
  // digitalWrite(LED_BUILTIN, LOW);
  // delay(1000);

  //
  if (mate_id == MATE_MOTION_SENSOR) {
    mate_motion_sensor_react();
  }

  vTaskDelay(10 / portTICK_RATE_MS);
}
