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


#ifndef SENSOR_H
#define SENSOR_H

#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "driver/adc.h"


/**
 * @brief   initialize adc for measure battery voltage
 *
 * @param  adc_channel  ADC channel connected to voltage divider
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_FAIL error
 */
esp_err_t sensor_battery_init(int32_t adc_channel);

/**
 * @brief   Get battery info, include voltage and charge state
 *
 * @param  voltage  battery voltage(mv)
 *
 * @return
 *     - ESP_OK Success
 */
esp_err_t sensor_battery_get_info(int32_t *voltage);



#endif