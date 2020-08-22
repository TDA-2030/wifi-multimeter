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


void checkConfig()
{
  ESP_LOGI(TAG, "Mode:                  ");
  switch (ina226_getMode())
  {
    case INA226_MODE_POWER_DOWN:      ESP_LOGI(TAG, "Power-Down"); break;
    case INA226_MODE_SHUNT_TRIG:      ESP_LOGI(TAG, "Shunt Voltage, Triggered"); break;
    case INA226_MODE_BUS_TRIG:        ESP_LOGI(TAG, "Bus Voltage, Triggered"); break;
    case INA226_MODE_SHUNT_BUS_TRIG:  ESP_LOGI(TAG, "Shunt and Bus, Triggered"); break;
    case INA226_MODE_ADC_OFF:         ESP_LOGI(TAG, "ADC Off"); break;
    case INA226_MODE_SHUNT_CONT:      ESP_LOGI(TAG, "Shunt Voltage, Continuous"); break;
    case INA226_MODE_BUS_CONT:        ESP_LOGI(TAG, "Bus Voltage, Continuous"); break;
    case INA226_MODE_SHUNT_BUS_CONT:  ESP_LOGI(TAG, "Shunt and Bus, Continuous"); break;
    default: ESP_LOGI(TAG, "unknown");
  }
  
  ESP_LOGI(TAG, "Samples average:       ");
  switch (ina226_getAverages())
  {
    case INA226_AVERAGES_1:           ESP_LOGI(TAG, "1 sample"); break;
    case INA226_AVERAGES_4:           ESP_LOGI(TAG, "4 samples"); break;
    case INA226_AVERAGES_16:          ESP_LOGI(TAG, "16 samples"); break;
    case INA226_AVERAGES_64:          ESP_LOGI(TAG, "64 samples"); break;
    case INA226_AVERAGES_128:         ESP_LOGI(TAG, "128 samples"); break;
    case INA226_AVERAGES_256:         ESP_LOGI(TAG, "256 samples"); break;
    case INA226_AVERAGES_512:         ESP_LOGI(TAG, "512 samples"); break;
    case INA226_AVERAGES_1024:        ESP_LOGI(TAG, "1024 samples"); break;
    default: ESP_LOGI(TAG, "unknown");
  }

  ESP_LOGI(TAG, "Bus conversion time:   ");
  switch (ina226_getBusConversionTime())
  {
    case INA226_BUS_CONV_TIME_140US:  ESP_LOGI(TAG, "140uS"); break;
    case INA226_BUS_CONV_TIME_204US:  ESP_LOGI(TAG, "204uS"); break;
    case INA226_BUS_CONV_TIME_332US:  ESP_LOGI(TAG, "332uS"); break;
    case INA226_BUS_CONV_TIME_588US:  ESP_LOGI(TAG, "558uS"); break;
    case INA226_BUS_CONV_TIME_1100US: ESP_LOGI(TAG, "1.100ms"); break;
    case INA226_BUS_CONV_TIME_2116US: ESP_LOGI(TAG, "2.116ms"); break;
    case INA226_BUS_CONV_TIME_4156US: ESP_LOGI(TAG, "4.156ms"); break;
    case INA226_BUS_CONV_TIME_8244US: ESP_LOGI(TAG, "8.244ms"); break;
    default: ESP_LOGI(TAG, "unknown");
  }

  ESP_LOGI(TAG, "Shunt conversion time: ");
  switch (ina226_getShuntConversionTime())
  {
    case INA226_SHUNT_CONV_TIME_140US:  ESP_LOGI(TAG, "140uS"); break;
    case INA226_SHUNT_CONV_TIME_204US:  ESP_LOGI(TAG, "204uS"); break;
    case INA226_SHUNT_CONV_TIME_332US:  ESP_LOGI(TAG, "332uS"); break;
    case INA226_SHUNT_CONV_TIME_588US:  ESP_LOGI(TAG, "558uS"); break;
    case INA226_SHUNT_CONV_TIME_1100US: ESP_LOGI(TAG, "1.100ms"); break;
    case INA226_SHUNT_CONV_TIME_2116US: ESP_LOGI(TAG, "2.116ms"); break;
    case INA226_SHUNT_CONV_TIME_4156US: ESP_LOGI(TAG, "4.156ms"); break;
    case INA226_SHUNT_CONV_TIME_8244US: ESP_LOGI(TAG, "8.244ms"); break;
    default: ESP_LOGI(TAG, "unknown");
  }
  
  ESP_LOGI(TAG, "Max possible current:  %f A", ina226_getMaxPossibleCurrent());
  ESP_LOGI(TAG, "Max current:           %f A", ina226_getMaxCurrent());
  ESP_LOGI(TAG, "Max shunt voltage:     %f V", ina226_getMaxShuntVoltage());
  ESP_LOGI(TAG, "Max power:             %f W", ina226_getMaxPower());

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
    speech_play_str("huan ying shi yong", 10);
    speech_set_volume(-8);

    sensor_battery_init(ADC_CHANNEL_0);

    /**< ADS1115 configuration*/
    ADS1x1x_config_t ads_config;
    ADS1x1x_init(&ads_config, ADS1115, ADS1x1x_I2C_ADDRESS_ADDR_TO_GND, MUX_SINGLE_1, PGA_4096);
    ADS1x1x_set_data_rate(&ads_config, DATA_RATE_ADS111x_128);

    /**< INA226 configuration*/
    ina226_begin(INA226_ADDRESS);
    ina226_configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
    ina226_calibrate(0.01, 4);
    checkConfig();// Display configuration

    /* Start the file server */
    start_web_server();

    while (1)
    {
        size_t _free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        float free = (float)_free / 1024.0f;
        // ESP_LOGI("main", "heap free=%f KB", free);

        {
            int32_t adc0, adc1, adc2, adc3;

            ADS1x1x_set_multiplexer(&ads_config, MUX_SINGLE_0);
            ADS1x1x_trigger_conversion(&ads_config);
            ADS1x1x_pollConversion(&ads_config, 500);
            adc0 = ADS1x1x_read_vol(&ads_config);

            ADS1x1x_set_multiplexer(&ads_config, MUX_SINGLE_1);
            ADS1x1x_trigger_conversion(&ads_config);
            ADS1x1x_pollConversion(&ads_config, 500);
            adc1 = ADS1x1x_read_vol(&ads_config);

            ADS1x1x_set_multiplexer(&ads_config, MUX_SINGLE_2);
            ADS1x1x_trigger_conversion(&ads_config);
            ADS1x1x_pollConversion(&ads_config, 500);
            adc2 = ADS1x1x_read_vol(&ads_config);

            ADS1x1x_set_multiplexer(&ads_config, MUX_SINGLE_3);
            ADS1x1x_trigger_conversion(&ads_config);
            ADS1x1x_pollConversion(&ads_config, 500);
            adc3 = ADS1x1x_read_vol(&ads_config);
            printf("AIN0: %d | ", adc0);
            printf("AIN1: %d | ", adc1);
            printf("AIN2: %d | ", adc2);
            printf("AIN3: %d | ", adc3);
            printf("\n");
            speech_play_num((float)adc0/10000.0f, "fu", NULL, 10);
            
        }
        float vol, curr;
        vol = ina226_readBusVoltage();
        curr = ina226_readShuntCurrent();
        printf("Bus voltage:   %f V\n", vol);
        printf("Bus power:     %f W\n", ina226_readBusPower());
        printf("Shunt voltage: %f V\n", ina226_readShuntVoltage());
        printf("Shunt current: %f A\n", curr);

        speech_play_num(curr, "an", NULL, 10);

        vTaskDelay(6000/portTICK_PERIOD_MS);
    }
    
}
