
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
} audios_data_t;

typedef struct {
    // The "RIFF" chunk descriptor
    uint8_t ChunkID[4];
    int32_t ChunkSize;
    uint8_t Format[4];
    // The "fmt" sub-chunk
    uint8_t Subchunk1ID[4];
    int32_t Subchunk1Size;
    int16_t AudioFormat;
    int16_t NumChannels;
    int32_t SampleRate;
    int32_t ByteRate;
    int16_t BlockAlign;
    int16_t BitsPerSample;
    // The "data" sub-chunk
    uint8_t Subchunk2ID[4];
    int32_t Subchunk2Size;
} wav_header_t;

static audios_data_t g_audios_head = {0};


/* Push an element into tail of the list */
static audios_data_t *list_push(audios_data_t *list, uint8_t *val, size_t len)
{
    audios_data_t *new_elem = malloc(sizeof(audios_data_t));
    SPEECH_CHECK(NULL != new_elem, "new list element malloc failed", NULL);
    new_elem->data = val;
    new_elem->len = len;
    list->next = new_elem;
    new_elem->next = NULL;
    g_audios_head.len++;  //表头的len字段用来统计表的长度
    return new_elem;
}

/* Length of list */
int List_length(audios_data_t *list)
{
    int n;

    for (n = 0; list; list = list->next) {
        n++;
    }

    return n;
}


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

    /**
     * read head of WAV file
     */
    wav_header_t wav_head;
    int ChunkSize = fread(&wav_head, 1, sizeof(wav_header_t), fd);

    /**
     * read wave data of WAV file
     */
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

    ESP_LOGI(TAG, "File reading complete, total: %ld bytes", write_num);
    return ESP_OK;
}

static esp_err_t add_num(audios_data_t **tail, uint8_t num)
{
    esp_err_t ret;
    SPEECH_CHECK(num <= 9, "number too large", ESP_FAIL);

    char path[32];
    uint8_t *data;
    size_t len;
    sprintf(path, "%s/%d.wav", BASE_PATH, num);
    ret = get_audio_data(path, &data, &len);
    SPEECH_CHECK(ESP_OK == ret, "get audio failed", ESP_FAIL);

    audios_data_t *p = list_push(*tail, data, len);

    if (NULL != p) {
        *tail = p;
    }

}

static esp_err_t add_char(audios_data_t **tail, char *str)
{
    esp_err_t ret;
    SPEECH_CHECK(strlen(str) < 10, "string too long", ESP_FAIL);

    char path[32];
    uint8_t *data;
    size_t len;
    sprintf(path, "%s/%s.wav", BASE_PATH, str);
    ret = get_audio_data(path, &data, &len);
    SPEECH_CHECK(ESP_OK == ret, "get audio failed", ESP_FAIL);

    audios_data_t *p = list_push(*tail, data, len);

    if (NULL != p) {
        *tail = p;
    }
}

static esp_err_t add_decimal(audios_data_t **tail, uint8_t index)
{
    char *dec = "ge";

    switch (index) {
        case 2:
            dec = "shi";
            break;

        case 3:
            dec = "bai";
            break;

        case 4:
            dec = "qian";
            break;

        default:
            break;
    }

    add_char(tail, dec);
}

static inline esp_err_t add_unit(audios_data_t **tail, char *str)
{
    add_char(tail, str);
}

static esp_err_t add_integral(audios_data_t **tail, char *number)
{
    SPEECH_CHECK(strlen(number) < 5, "number too long", ESP_FAIL);

    char s[16] = {0};
    strcpy(s, number);
    ESP_LOGI(TAG, "add integral number: %s", s);

    int8_t i = 0;
    int8_t last_zero = -1;
    const uint8_t len_s = strlen(s);

    while (i < len_s) {
        uint8_t num = s[i] - '0';

        if (last_zero == -1 || num != 0) {
            add_num(tail, num);
        }

        if (num != 0) {
            last_zero = -1;
            add_decimal(tail, len_s - i);
        } else {
            last_zero = i;
        }

        /**
         * check is all zero
         */
        uint8_t zero_num = 0;

        for (size_t j = i + 1; j < len_s; j++) {
            if (s[j] == '0') {
                zero_num++;
            }
        }

        if (zero_num == (len_s - i - 1)) {
            break;
        }

        i++;
    }
}

// static esp_err_t synthesis(float num, char *unit1, char *unit2)
// {
//     audios_data_t *audio_list = &g_audios_head;
//     if (num < 1000){
//         s = str(num)
//         s = s[:5]
//         Integral = s.split('.')[0]
//         Fractional = s.split('.')[1]

//         add_integral(sound, int(Integral))

//         add_char(sound, 'dian');

//         for i in range(0, len(Fractional)){
//             add_num(sound, int(Fractional[i]))
//         }
//     }
//     else{
//         add_integral(, num);
//     }

//     add_unit(sound, unit1);
//     if (NULL != unit2){
//         add_unit(sound, unit2);
//     }
//     return ESP_OK;
// }

esp_err_t speech_start(float num, char *unit1, char *unit2)
{
    return ESP_OK;
}

