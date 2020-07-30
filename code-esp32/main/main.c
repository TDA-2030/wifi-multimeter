/* HTTP File Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <sys/param.h>

#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "web_server.h"
#include "speech.h"
#include "blufi.h"
#include "mdns.h"
#include "sensor.h"
#include "file_manage.h"

/* This example demonstrates how to create file server
 * using esp_http_server. This file has only startup code.
 * Look in file_server.c for the implementation */

static const char *TAG="multimeter";
#define HOST_NAME "my-meter"



static void initialise_mdns(void)
{
    mdns_init();
    mdns_hostname_set(HOST_NAME);
    mdns_instance_name_set(HOST_NAME);
    ESP_LOGI(TAG, "mDNS host name set to %s", HOST_NAME);

    mdns_txt_item_t serviceTxtData[] = {
        {"board", "esp32"},
        {"path", "/"}
    };

    ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
                                     sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));
}


void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());

    /* Initialize file storage */
    ESP_ERROR_CHECK(fm_init());

    // /**< Initialize the BluFi */
    blufi_network_init();
    bool configured;
    blufi_is_configured(&configured);

    if (!configured) {
        blufi_start();
    }

    ESP_LOGI(TAG, "Wait for connect");
    blufi_wait_connection(portMAX_DELAY);

    if (!configured) {
        blufi_stop();
    }

    initialise_mdns();

    speech_init();

    speech_play_num(1214, "wei", "an", 10);
    speech_play_str(10, "%s,%s,%s", "huan", "ying", "shi");

    sensor_battery_init(ADC_CHANNEL_0);

    /* Start the file server */
    start_web_server();

    while (1)
    {
        size_t _free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        float free = (float)_free / 1024.0f;
        ESP_LOGI("main", "heap free=%f KB", free);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    
}
