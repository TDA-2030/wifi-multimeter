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
#include "captive_portal.h"
#include "mdns.h"
#include "measure.h"
#include "file_manage.h"

#include "ads1x1x.h"
#include "ina226.h"


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
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Initialize file storage */
    ESP_ERROR_CHECK(fm_init());

    bool is_configured;
    captive_portal_start("ESP_WEB_CONFIG", NULL, &is_configured);
    wifi_config_t wifi_config;
    esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_config);
    if (is_configured) {
        ESP_LOGI(TAG, "SSID:%s, PASSWORD:%s", wifi_config.sta.ssid, wifi_config.sta.password);
    } else {
        ESP_LOGI(TAG, "WiFi not configured, started a configuration webpage");
    }
    captive_portal_wait(portMAX_DELAY);

    initialise_mdns();

    // speech_init();

    // speech_play_num(1214, "wei", "an", 10);
    // speech_play_str("huan ying shi yong", 10);
    // speech_set_volume(-8);

    /**< ADS1115 configuration*/
    // ADS1x1x_config_t ads_config;
    // ADS1x1x_init(&ads_config, ADS1115, ADS1x1x_I2C_ADDRESS_ADDR_TO_GND, MUX_SINGLE_1, PGA_4096);
    // ADS1x1x_set_data_rate(&ads_config, DATA_RATE_ADS111x_128);

    /**< INA226 configuration*/
    ina226_begin(INA226_ADDRESS);
    ina226_configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_8244US, INA226_SHUNT_CONV_TIME_8244US, INA226_MODE_SHUNT_BUS_CONT);
    ina226_calibrate(0.1, 0.8192);
    // checkConfig();// Display configuration

    /* Start the file server */
    start_web_server();

    while (1)
    {
        size_t _free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        float free = (float)_free / 1024.0f;
        // ESP_LOGI("main", "heap free=%f KB", free);

        // if(0){
        //     int32_t adc0, adc1, adc2, adc3;

        //     ADS1x1x_set_multiplexer(&ads_config, MUX_SINGLE_0);
        //     ADS1x1x_trigger_conversion(&ads_config);
        //     ADS1x1x_pollConversion(&ads_config, 500);
        //     adc0 = ADS1x1x_read_vol(&ads_config);

        //     ADS1x1x_set_multiplexer(&ads_config, MUX_SINGLE_1);
        //     ADS1x1x_trigger_conversion(&ads_config);
        //     ADS1x1x_pollConversion(&ads_config, 500);
        //     adc1 = ADS1x1x_read_vol(&ads_config);

        //     ADS1x1x_set_multiplexer(&ads_config, MUX_SINGLE_2);
        //     ADS1x1x_trigger_conversion(&ads_config);
        //     ADS1x1x_pollConversion(&ads_config, 500);
        //     adc2 = ADS1x1x_read_vol(&ads_config);

        //     ADS1x1x_set_multiplexer(&ads_config, MUX_SINGLE_3);
        //     ADS1x1x_trigger_conversion(&ads_config);
        //     ADS1x1x_pollConversion(&ads_config, 500);
        //     adc3 = ADS1x1x_read_vol(&ads_config);
        //     printf("AIN0: %d | ", adc0);
        //     printf("AIN1: %d | ", adc1);
        //     printf("AIN2: %d | ", adc2);
        //     printf("AIN3: %d | ", adc3);
        //     printf("\n\n");
        //     speech_play_num((float)adc0/10000.0f, "fu", NULL, 10);
            
        // }
        float vol, curr;
        vol = ina226_readBusVoltage();
        curr = ina226_readShuntCurrent();
        printf("Bus voltage:   %f V\n", vol);
        printf("Bus power:     %f W\n", ina226_readBusPower());
        printf("Shunt voltage: %f V\n", ina226_readShuntVoltage());
        printf("Shunt current: %f A\n", curr);
        

        // speech_play_num(curr, "an", NULL, 10);

        vTaskDelay(10000/portTICK_PERIOD_MS);
    }
    
}
