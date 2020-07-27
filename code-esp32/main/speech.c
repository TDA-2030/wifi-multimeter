
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_vfs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

static const char *TAG = "speech";

#define SPEECH_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }

#define BASE_PATH "/spiffs"
#define AUDIO_NUM_MAX 16
typedef struct {
    uint8_t *data;
    size_t len;
    audios_data_t *next;
}audios_data_t;

static audios_data_t *audios_head = NULL;

static void pwm_audio_task(void *arg)
{

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


esp_err_t speech_init(void)
{
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

    ESP_LOGI(TAG, "play init");
    pwm_audio_set_param(16000, 16, 1);
    pwm_audio_start();
    pwm_audio_set_volume(0);

    return ESP_OK;
}

static esp_err_t get_audio_data(const char *filepath, uint8_t **out_data, size_t *out_length)
{
    FILE *fd = NULL;
    struct stat file_stat;

    if (stat(filepath, &file_stat) == -1) {
        ESP_LOGE(TAG, "Failed to stat file : %s", filepath);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "file info: %s (%ld bytes)...", filepath, file_stat.st_size);
    *out_length = file_stat.st_size;
    fd = fopen(filepath, "r");

    if (NULL == fd) {
        ESP_LOGE(TAG, "Failed to read existing file : %s", filepath);
        return ESP_FAIL;
    }

    *out_data = malloc(*out_length);

    if (NULL == *out_data) {
        ESP_LOGE(TAG, "audio data buffer malloc failed");
        fclose(fd);
        return ESP_FAIL;
    }

    size_t chunksize;
    size_t write_num = 0;

    do {
        /* Read file in chunks into the scratch buffer */
        chunksize = fread((*out_data) + write_num, 1, 4096, fd);

        if (chunksize <= 0) {
            break;
        }

        write_num += chunksize;
    } while (*out_length > write_num);

    fclose(fd);

    *out_length = write_num;
    ESP_LOGI(TAG, "File reading complete, total: %ld bytes", write_num);
    return ESP_OK;
}

static esp_err_t add_num(audios_data_t **audios, uint8_t num)
{
    SPEECH_CHECK(num <= 9, "number too large", ESP_FAIL);
    *audios = malloc(sizeof(audios_data_t));
    memset(*audios, 0, sizeof(audios_data_t));
    SPEECH_CHECK(NULL != *audios, "audios_data malloc failed", ESP_FAIL);
    
    char path[32];
    sprintf(path, "%s/%d.wav", BASE_PATH, num);
    get_audio_data(path, &(*audios->data) ,&(*audios->len));
}

// static esp_err_t add_char(sound, char){
//     print("add char %s" % char)
//     name = 'voice/' + char + '.wav'
//     sound.append(AudioSegment.from_file(name))
// }
// static esp_err_t add_decimal(sound, index){
//     char = 'ge'
//     if index == 1:
//         return
//     if index == 2:
//         char = 'shi'
//     elif index == 3:
//         char = 'bai'
//     elif index == 4:
//         char = 'qian'
//     add_char(sound, char)
// }
// static esp_err_t add_unit(sound, unit){
//     add_char(sound, unit)
// }

// static esp_err_t add_integral(sound, num){
//     s = str(num)
//     s = s[:4]

//     i = 0
//     last_zero = -1
//     while i < len(s):
//         num = int(s[i])

//         if last_zero == -1 or num != 0:
//             add_num(sound, num)
//         if num != 0:
//             last_zero = -1
//             add_decimal(sound, len(s) - i)
//         else:
//             last_zero = i

//         # check is all zero
//         zero_num = 0
//         for j in range(i + 1, len(s)):
//             if int(s[j]) == 0:
//                 zero_num = zero_num + 1
//         if zero_num == (len(s) - i - 1):
//             break
//         i = i + 1
// }

esp_err_t speech_start(float num, char *unit1, char *unit2)
{
    return ESP_OK;
}

