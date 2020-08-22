#ifndef SPEECH_H_
#define SPEECH_H_


esp_err_t speech_init(void);
esp_err_t speech_play_num(float num, char *unit1, char *unit2, TickType_t xTicksToWait);
esp_err_t speech_play_str(const char *str, TickType_t xTicksToWait);
esp_err_t speech_set_volume(int8_t val);
#endif