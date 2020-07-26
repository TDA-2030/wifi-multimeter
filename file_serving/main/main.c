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

static void pwm_audio_task(void *arg)
{
    wave_array     = wave_get();
    wave_size      = wave_get_size();
    wave_framerate = wave_get_framerate();
    wave_bits      = wave_get_bits();
    wave_ch        = wave_get_ch();

    pwm_audio_config_t pac;
    pac.duty_resolution    = LEDC_TIMER_10_BIT;
    pac.gpio_num_left      = 25;
    pac.ledc_channel_left  = LEDC_CHANNEL_0;
    pac.gpio_num_right     = -1;
    pac.ledc_channel_right = LEDC_CHANNEL_1;
    pac.ledc_timer_sel     = LEDC_TIMER_0;
    pac.tg_num             = TIMER_GROUP_0;
    pac.timer_num          = TIMER_0;
    pac.ringbuf_len        = 1024 * 8;
    pwm_audio_init(&pac);

    uint32_t index = 0;
    size_t cnt;
    uint32_t block_w = 2048;
    ESP_LOGI(TAG, "play init");
    pwm_audio_set_param(wave_framerate, wave_bits, wave_ch);
    pwm_audio_start();
    pwm_audio_set_volume(0);

    while (1) {
        if (index < wave_size) {
            if ((wave_size - index) < block_w) {
                block_w = wave_size - index;
            }

            pwm_audio_write((uint8_t *)wave_array + index, block_w, &cnt, 5000 / portTICK_PERIOD_MS);
            ESP_LOGI(TAG, "write [%d] [%d]", block_w, cnt);
            index += cnt;
        } else {

            ESP_LOGW(TAG, "play completed");
#ifdef REPEAT_PLAY
            index = 0;
            block_w = 2048;
            pwm_audio_stop();
            vTaskDelay(2500 / portTICK_PERIOD_MS);
            pwm_audio_start();
            ESP_LOGW(TAG, "play start");
#else
            pwm_audio_stop();
            vTaskDelay(portMAX_DELAY);
#endif
        }

        vTaskDelay(6 / portTICK_PERIOD_MS);
    }
}

esp_err_t start_web_server(void);

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /**< Initialize the BluFi */
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

    /* Initialize file storage */
    ESP_ERROR_CHECK(init_spiffs());

    sensor_battery_init(ADC_CHANNEL_6);

    /* Start the file server */
    start_web_server();
}
