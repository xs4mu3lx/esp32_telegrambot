/*

██   ██ ████████ ████████ ██████      ████████  █████  ███████ ██   ██ 
██   ██    ██       ██    ██   ██        ██    ██   ██ ██      ██  ██  
███████    ██       ██    ██████         ██    ███████ ███████ █████   
██   ██    ██       ██    ██             ██    ██   ██      ██ ██  ██  
██   ██    ██       ██    ██             ██    ██   ██ ███████ ██   ██ 
 

 - Code by: s4mu3l
 - last update: 30/10/24
 
 
 */
/*═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════*/

#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "telegram_bot_data.h"

/*═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════*/

DATA_UPDATE_t data_update_t;
SYSTEM_INFO_t system_info_t;

/*
 * https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/src/TelegramCertificate.h
*/
extern const char telegram_certificate_pem_start[] asm("_binary_telegram_certificate_pem_start");
extern const char telegram_certificate_pem_end[] asm("_binary_telegram_certificate_pem_end");

/*═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════*/

void http_get_post(char *chat_id, char *str_send, uint8_t metodo)
{
    char url[1024] = "https://api.telegram.org/bot";
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    char post_data[1024] = "";

    esp_http_client_config_t config = {
        .url = "https://api.telegram.org",
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .event_handler = _http_event_handler,
        .cert_pem = telegram_certificate_pem_start,
        .user_data = output_buffer,
    };

    ESP_LOGW(TAG_HTTP_GET_POST, "Start");
    esp_http_client_handle_t client = esp_http_client_init(&config);

    if (metodo == 1)
    {
        /* POST */
        snprintf(url, sizeof(url), "https://api.telegram.org/bot%s/sendMessage", TOKEN);
        sprintf(post_data, "{\"chat_id\":%s,\"text\":\"%s\"}", chat_id, str_send);

        ESP_LOGW(TAG_SEND_MSS, "Json: %s", post_data);

        esp_http_client_set_url(client, url);
        esp_http_client_set_method(client, HTTP_METHOD_POST);
        esp_http_client_set_header(client, "Content-Type", "application/json");
        esp_http_client_set_post_field(client, post_data, strlen(post_data));

        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK)
        {
            ESP_LOGI(TAG_SEND_MSS, "HTTP POST Status = %d, content_length = %" PRId64, esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
            printf("Perform output: %s\n", output_buffer);
        }

        else
        {
            ESP_LOGE(TAG_SEND_MSS, "HTTP POST failed: %s", esp_err_to_name(err));
        }
    }

    else
    {
        /* GET */
        snprintf(url, sizeof(url), "https://api.telegram.org/bot%s/getUpdates?offset=%d&limit=1", TOKEN, data_update_t.last_update_id + 1);

        esp_http_client_set_url(client, url);
        esp_http_client_set_method(client, HTTP_METHOD_GET);

        esp_err_t err = esp_http_client_perform(client);

        if (err == ESP_OK)
    {
        ESP_LOGI(TAG_GET_UPDATE, "HTTPS Status = %d, content_length = %lld", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
        printf("Perform output: %s\n", output_buffer);

        char *text_pos = strstr(output_buffer, "\"text\"");

        if (text_pos != NULL)
        {
            text_pos += 7;
            text_pos = strchr(text_pos, '\"');

            if (text_pos != NULL)
            {
                text_pos++;
                //char text_value[256];
                sscanf(text_pos, "%[^\"]", data_update_t.command_bot);
                //printf("%s\n", command_bot);
            }
            else
            {
                printf("The value was not found after \"text\".\n");
            }
        }
        else
        {
            printf("\"text\" not found in string JSON.\n");
        }

        char *id_post = strstr(output_buffer, "\"chat\":{\"id\":");

        if (id_post != NULL)
        {
            id_post += strlen("\"chat\":{\"id\":");
            
            char chatIdstr[20];
            sscanf(id_post, "%[^,]", chatIdstr);

            char *comma = strchr(chatIdstr, ',');
            while (comma != NULL)
            {
                memmove(comma, comma + 1, strlen(comma));
                comma = strchr(chatIdstr, ',');
            }

            snprintf(data_update_t.chat_id, sizeof(data_update_t.chat_id), "%s", chatIdstr);

            
        }
        else
        {
            printf("\"chat\":{\"id\": not found in string JSON.\n");
        }


        cJSON *json_root = cJSON_Parse(output_buffer);

        if (json_root == NULL) printf("Error al parsear el JSON.\n");

        cJSON *result_array = cJSON_GetObjectItem(json_root, "result");
        cJSON *update_id_object = cJSON_GetObjectItem(result_array->child, "update_id");
        if (update_id_object != NULL)
        {
            int update_id = update_id_object->valueint;
            data_update_t.last_update_id  = update_id;

            printf("Chat_id: %s\n", data_update_t.chat_id);
            printf("LastUpdateId: %d | Text value: %s\n", data_update_t.last_update_id, data_update_t.command_bot);
        }
        else
        {
            printf("No found.\n");
            
        }

            cJSON_Delete(json_root);
        }
    }
    
    ESP_LOGW(TAG_HTTP_GET_POST, "Clean");
    esp_http_client_cleanup(client);
    ESP_LOGI(TAG_HTTP_GET_POST, "esp_get_free_heap_size: %lu", esp_get_free_heap_size());
}


void http_task(void *pvParameters)
{

    char msg_to_send[1024] = "";
    char chat_id[256] = "";
    int cnt = 20;
    float tsens_value;
    system_info_t.project_name = "TelegramBot";
    system_info_t.build_date = "30/10/24 | 5:15";
    system_info_t.version_fwr = "v1.0";
    system_info_t.esp_idf_v = esp_get_idf_version();
    system_info_t.temp = 0.0;

    char msg_reset[1024] = "";
    snprintf(msg_reset, sizeof(msg_reset), "⚞ W4RN1NG ⚟\n\nRESTART\n\nRestart reason ->  [%d]", esp_reset_reason());

    ESP_LOGI(TAG_HTTP_TASK, "Install temperature sensor, expected temp ranger range: 10~50 ℃");
    temperature_sensor_handle_t temp_sensor = NULL;
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));

    ESP_LOGI(TAG_HTTP_TASK, "Enable temperature sensor");
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));

    wifi_init_sta();

    ESP_LOGW(TAG_HTTP_CLIENT_HANDLER, "Wait 2 second before start");
    vTaskDelay(pdMS_TO_TICKS(2000));

    http_get_post(CHAT_ID, msg_reset, 1);


    while (1)
    {

        http_get_post(NULL, NULL, 0);

        for (int i = 0; data_update_t.command_bot[i]; i++)
        {
            data_update_t.command_bot[i] = tolower(data_update_t.command_bot[i]);
        }

        if (strcmp(data_update_t.command_bot, "/start") == 0)
        {
            snprintf(msg_to_send, sizeof(msg_to_send), "⚞ WℇℒC0ℳℇ ⚟ \n\nThese are my basic commands.\n\n/sysinfo  ->  send system info\n/state  ->  send the current status of the device\n/restart  ->  reboot system\n\n[s3bot]  by s4mu3l");
            http_get_post(data_update_t.chat_id, msg_to_send, 1);
            snprintf(data_update_t.command_bot, sizeof(data_update_t.command_bot), " ");
        }
        else if (strcmp(data_update_t.command_bot, "/sysinfo") == 0)
        {
            snprintf(msg_to_send, sizeof(msg_to_send), "⚞ SISTEM ⚟\n\nProject name:  %s\n\nBuild date:  %s\n\nFirmware version:  %s\n\nEsp-idf version:  %s\n\n[s3bot]  by s4mu3l", system_info_t.project_name, system_info_t.build_date, system_info_t.version_fwr, system_info_t.esp_idf_v);
            http_get_post(data_update_t.chat_id, msg_to_send, 1);
            snprintf(data_update_t.command_bot, sizeof(data_update_t.command_bot), " ");
        }

        else if (strcmp(data_update_t.command_bot, "/state") == 0)
        {
            ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &system_info_t.temp));
            snprintf(msg_to_send, sizeof(msg_to_send), "⚞ STATE ⚟\n\nLast update:  %d\n\nTemp CPU:  %d ℃\n\nYour chat-id:  %s\n\n[s3bot]  by s4mu3l", data_update_t.last_update_id, (int16_t)system_info_t.temp, data_update_t.chat_id);
            http_get_post(data_update_t.chat_id, msg_to_send, 1);
            snprintf(data_update_t.command_bot, sizeof(data_update_t.command_bot), " ");
        }

        else if (strcmp(data_update_t.command_bot, "/restart") == 0)
        {
            http_get_post(data_update_t.chat_id, "Restarting...", 1);
            snprintf(data_update_t.command_bot, sizeof(data_update_t.command_bot), " ");
            http_get_post(NULL, NULL, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
            esp_restart();
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    vTaskDelete(NULL);
    
}

/*═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════*/

