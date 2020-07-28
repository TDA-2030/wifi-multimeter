#ifndef SPEECH_H_
#define SPEECH_H_


esp_err_t speech_init(void);
esp_err_t speech_start(float num, char *unit1, char *unit2, TickType_t xTicksToWait);

#endif