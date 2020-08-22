
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

#include <stdarg.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "pwm_audio.h"
#include "speech.h"
#include "file_manage.h"

static const char *TAG = "speech";

#define SPEECH_CHECK(a, str, ret_val) \
    if (!(a)) { \
        ESP_LOGE(TAG,"%s(%d): %s", __FUNCTION__, __LINE__, str); \
        return (ret_val); \
    }


typedef struct audios_data_t{
    uint8_t *data;
    size_t len;
    struct audios_data_t *next;
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

typedef enum {
    SPEECH_TYPE_NUM,
    SPEECH_TYPE_STR,
}speech_type_t;

#define SPEECH_STRING_MAX_LENGTH 32
typedef struct {
    speech_type_t type;
    float num;
    char unit1[SPEECH_STRING_MAX_LENGTH];  /**< storage unit string or speech string */
    char unit2[4];   /**< storage unit string only */
} speech_data_t;

static audios_data_t g_audios_head = {0};
static QueueHandle_t g_speech_queue = NULL;

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


static esp_err_t get_audio_data(const char *filepath, uint8_t **out_data, size_t *out_length)
{
    FILE *fd = NULL;
    struct stat file_stat;

    if (stat(filepath, &file_stat) == -1) {
        ESP_LOGE(TAG, "Failed to stat file : %s", filepath);
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "file stat info: %s (%ld bytes)...", filepath, file_stat.st_size);
    *out_length = file_stat.st_size;
    fd = fopen(filepath, "r");

    if (NULL == fd) {
        ESP_LOGE(TAG, "Failed to read existing file : %s", filepath);
        return ESP_FAIL;
    }

    *out_length -= sizeof(wav_header_t);
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
    int chunksize = fread(&wav_head, 1, sizeof(wav_header_t), fd);
    ESP_LOGD(TAG, "frame_rate=%d, ch=%d, width=%d", wav_head.SampleRate, wav_head.NumChannels, wav_head.BitsPerSample);

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
    if(*out_length != write_num){
        ESP_LOGE(TAG, "file read incorrect");
    }

    ESP_LOGD(TAG, "File reading complete, total: %d bytes", write_num);
    return ESP_OK;
}

static esp_err_t add_num(audios_data_t **tail, uint8_t num)
{
    esp_err_t ret;
    SPEECH_CHECK(num <= 9, "number too large", ESP_FAIL);

    char path[32];
    uint8_t *data;
    size_t len;
    fs_info_t *info;
    fm_get_info(&info);
    sprintf(path, "%s/%d.wav", info->base_path, num);
    ret = get_audio_data(path, &data, &len);
    SPEECH_CHECK(ESP_OK == ret, "get audio failed", ESP_FAIL);

    audios_data_t *p = list_push(*tail, data, len);

    if (NULL != p) {
        *tail = p;
        return ESP_OK;
    }
    return ESP_FAIL;
}

static esp_err_t add_char(audios_data_t **tail, const char *str)
{
    esp_err_t ret;
    SPEECH_CHECK(strlen(str) < 10, "string too long", ESP_FAIL);

    char path[32];
    uint8_t *data;
    size_t len;
    fs_info_t *info;
    fm_get_info(&info);
    sprintf(path, "%s/%s.wav", info->base_path, str);
    ret = get_audio_data(path, &data, &len);
    SPEECH_CHECK(ESP_OK == ret, "get audio failed", ESP_FAIL);

    audios_data_t *p = list_push(*tail, data, len);

    if (NULL != p) {
        *tail = p;
        return ESP_OK;
    }
    return ESP_FAIL;
}

static esp_err_t add_decimal(audios_data_t **tail, uint8_t index)
{
    char *dec = "ge";

    switch (index) {
        case 1:
            
            return ESP_OK;
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

    return add_char(tail, dec);
}

static inline esp_err_t add_unit(audios_data_t **tail, char *str)
{
    return add_char(tail, str);
}

static esp_err_t add_integral(audios_data_t **tail, char *number)
{
    SPEECH_CHECK(strlen(number) < 5, "number too long", ESP_FAIL);

    char s[16] = {0};
    strcpy(s, number);

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
    return ESP_OK;
}

static esp_err_t synthesis_num(float num, char *unit1, char *unit2)
{
    SPEECH_CHECK(num < 9999.9f && num > -9999.9f, "number too large", ESP_FAIL);
    
    audios_data_t *audio_list = &g_audios_head;
    char s[16]={0};

    if(num < 0){
        add_char(&audio_list, "fu");
        num = -num;
    }

    if (num < 1000){
        sprintf(s, "%f", num);
        s[5]='\0';
        char *integral = s;
        
        char *dot = strchr(s, '.');
        *dot = '\0';
        char *fractional = dot+1;
        uint8_t len_fractional = strlen(fractional);
        ESP_LOGD(TAG, "number split to [%s.%s]\n", integral, fractional);

        add_integral(&audio_list, integral);
        add_char(&audio_list, "dian");

        for(int i=0; i<len_fractional; i++){
            add_num(&audio_list, fractional[i]-'0');
        }
    }
    else{
        sprintf(s, "%4d", (uint32_t)num);
        add_integral(&audio_list, s);
    }

    if ('\0' != *unit1){
        add_unit(&audio_list, unit1);
    }
    if ('\0' != *unit2){
        add_unit(&audio_list, unit2);
    }
    return ESP_OK;
}

static esp_err_t synthesis_str(const char *str)
{
    SPEECH_CHECK(NULL != str, "string invalid", ESP_FAIL);
    esp_err_t ret;
    audios_data_t *audio_list = &g_audios_head;
    char *p_s, *p_e;
    p_s = (char*)str;
    p_e = p_s;
    while(1){
        
        if(*p_e == ' '){ /**< find a space */
            *p_e = '\0';
            add_char(&audio_list, p_s);
            p_s = p_e + 1;
        }else if(*p_e == '\0'){ /**< end by terminator */
            add_char(&audio_list, p_s);
            break;
        }
        p_e++;
    }
    return ret;
}

static void pwm_audio_task(void *arg)
{
    uint8_t *wave_array=NULL;
    size_t cnt;
    size_t block_w;

    pwm_audio_set_volume(-14);

    while(1){
        speech_data_t speech_data={0};
        if(g_speech_queue == NULL){
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        if(pdTRUE != xQueueReceive(g_speech_queue, &speech_data, portMAX_DELAY))
  		{
  			vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
  		}
        
        if(SPEECH_TYPE_NUM == speech_data.type){
            synthesis_num(speech_data.num, speech_data.unit1, speech_data.unit2);
        }else if (SPEECH_TYPE_STR == speech_data.type){
            synthesis_str(speech_data.unit1);
        }
        
        ESP_LOGD(TAG, "list len=%d, sizeof(audios_data_t)=%d\n", g_audios_head.len, sizeof(audios_data_t));

        audios_data_t *list=NULL;
        audios_data_t *nt = g_audios_head.next;
        while (1) {
            
            if(NULL != nt){
                list = nt;
                wave_array = list->data;
                block_w = list->len;
                pwm_audio_write(wave_array, block_w, &cnt, 20000 / portTICK_PERIOD_MS);
                ESP_LOGD(TAG, "write [%d] [%d]", block_w, cnt);
                nt = list->next;
                free(list->data);
                free(list);
                g_audios_head.len--;
                
            } else {
                vTaskDelay(600 / portTICK_PERIOD_MS);
                // pwm_audio_wait_complete(portMAX_DELAY);
                ESP_LOGD(TAG, "play completed");
                break;
            }
        }
    }
}

esp_err_t speech_init(void)
{
    g_speech_queue = xQueueCreate( 10, sizeof(speech_data_t) );
    SPEECH_CHECK(g_speech_queue != NULL, "g_speech_queue create failed", ESP_FAIL);

    pwm_audio_config_t pac;
    pac.duty_resolution    = LEDC_TIMER_10_BIT;
    pac.gpio_num_left      = 25;
    pac.ledc_channel_left  = LEDC_CHANNEL_0;
    pac.gpio_num_right     = 26;
    pac.ledc_channel_right = LEDC_CHANNEL_1;
    pac.ledc_timer_sel     = LEDC_TIMER_0;
    pac.tg_num             = TIMER_GROUP_0;
    pac.timer_num          = TIMER_0;
    pac.ringbuf_len        = 1024 * 8;
    pwm_audio_init(&pac);

    ESP_LOGI(TAG, "audio play init");
    pwm_audio_set_param(16000, 16, 1);
    pwm_audio_start();
    pwm_audio_set_volume(0);

    xTaskCreatePinnedToCore(pwm_audio_task, "pwm_audio_task", 1024 * 3, NULL, 8, NULL, 1);

    return ESP_OK;
}

esp_err_t speech_play_num(float num, char *unit1, char *unit2, TickType_t xTicksToWait)
{
    speech_data_t sd={0};
    sd.type = SPEECH_TYPE_NUM;
    sd.num = num;
    if (NULL != unit1){
        strncpy(sd.unit1, unit1, 4);
        sd.unit1[3] = '\0';
    }
    if (NULL != unit2){
        strncpy(sd.unit2, unit2, 4);
        sd.unit2[3] = '\0';
    }
    
    if(pdTRUE != xQueueSend( g_speech_queue, &sd, xTicksToWait)){
        ESP_LOGE(TAG, "send speech data failed");
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

esp_err_t speech_play_str(const char *str, TickType_t xTicksToWait)
{
    SPEECH_CHECK(NULL != str, "string invalid", ESP_FAIL);
    SPEECH_CHECK(SPEECH_STRING_MAX_LENGTH-1 > strlen(str), "string too long", ESP_FAIL)

    speech_data_t sd={0};
    sd.type = SPEECH_TYPE_STR;
    ESP_LOGI(TAG, "play string [%s]\n", str);
    strcpy(sd.unit1, str);

    if(pdTRUE != xQueueSend( g_speech_queue, &sd, xTicksToWait)){
        ESP_LOGE(TAG, "send speech data failed");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t speech_set_volume(int8_t val)
{
    return pwm_audio_set_volume(val);
}