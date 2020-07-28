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

/* This example demonstrates how to create file server
 * using esp_http_server. This file has only startup code.
 * Look in file_server.c for the implementation */

static const char *TAG="multimeter";
#define HOST_NAME "my-meter"

/* Function to initialize SPIFFS */
static esp_err_t init_spiffs(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,   // This decides the maximum number of files that can be created on the storage
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

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

    // ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Initialize file storage */
    ESP_ERROR_CHECK(init_spiffs());

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

    speech_start(1000, "hao", "an", 10 / portTICK_PERIOD_MS);

    speech_start(1200, "hao", "an", 10 / portTICK_PERIOD_MS);
    speech_start(1020, "hao", "an", 10 / portTICK_PERIOD_MS);
    speech_start(1002, "hao", "an", 10 / portTICK_PERIOD_MS);

    speech_start(1230, "hao", "fu", 10 / portTICK_PERIOD_MS);
    speech_start(1203, "hao", "fu", 10 / portTICK_PERIOD_MS);
    speech_start(1023, "hao", "fu", 10 / portTICK_PERIOD_MS);

    speech_start(74.5, "hao", "fu", 10 / portTICK_PERIOD_MS);
    speech_start(3.769, "hao", "fu", 10 / portTICK_PERIOD_MS);
    
    sensor_battery_init(ADC_CHANNEL_6);

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
