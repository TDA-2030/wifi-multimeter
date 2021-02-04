// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "ina226.h"
#include "ads1x1x.h"
#include "measure.h"

static const char *TAG = "measure";




static void sensor_battery_task(void *arg)
{
    // static TimerHandle_t tmr;
    // tmr = xTimerCreate("adc measure",
    //                    10 / portTICK_PERIOD_MS,
    //                    pdTRUE,
    //                    NULL,
    //                    adc_proid_sample);

    // if (tmr) {
    //     xTimerStart(tmr, 0);
    // }

    while (1) {
        // ESP_LOGI(TAG, "battery voltage: %dmv", g_vol_bat);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

esp_err_t sensor_battery_init(int32_t adc_channel)
{
   
    xTaskCreate(sensor_battery_task, "battery", 1024 * 2, NULL, 3, NULL);

    return ESP_OK;
}



