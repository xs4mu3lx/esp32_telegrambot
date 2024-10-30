#ifndef CONFIG_H_
#define CONFIG_H_

/*

 ██████  ██████  ███    ██ ███████ ██  ██████  
██      ██    ██ ████   ██ ██      ██ ██       
██      ██    ██ ██ ██  ██ █████   ██ ██   ███ 
██      ██    ██ ██  ██ ██ ██      ██ ██    ██ 
 ██████  ██████  ██   ████ ██      ██  ██████  
 

 - Code by: s4mu3l
 - last update: 30/10/24
 
 
 */
/*═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_wifi.h"
#include "esp_http_client.h"
#include "driver/temperature_sensor.h"
#include "cJSON.h"

/*═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════*/

/* Wifi Config */

#define ESP_WIFI_SSID "myssid"
#define ESP_WIFI_PASS "mypassword"

#define ESP_MAXIMUM_RETRY 10
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

/*═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════*/

/* Telegram Config */

/*HTTP buffer*/
#define MAX_HTTP_RECV_BUFFER 1024
#define MAX_HTTP_OUTPUT_BUFFER 2048

/*Telegram configuration*/
#define TOKEN "your token bot"
#define chat_ID "your chat-id"

#define TAG_HTTP_TASK            "[ http_task ]"
#define TAG_HTTP_CLIENT_HANDLER  "[ http client Handler ]"
#define TAG_SEND_MSS             "[ sendMessage ]"
#define TAG_GET_UPDATE           "[ getUpdates ]"
#define TAG_HTTP_GET_POST        "[ http_get_pos ]"
#define TAG_WIFI_STA             "[ wifi station ]"

/*═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════*/

esp_err_t _http_event_handler(esp_http_client_event_t *evt);
void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void wifi_init_sta(void);

/*═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════*/

#endif /* CONFIG_H_ */