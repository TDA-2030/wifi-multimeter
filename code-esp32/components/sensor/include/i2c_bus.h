
#include "esp_log.h"
#include "driver/i2c.h"

esp_err_t i2c_master_init(void);
esp_err_t i2c_set_address(uint16_t addr);
esp_err_t i2c_master_read_slave(uint8_t *data_rd, size_t size);
esp_err_t i2c_master_write_slave(uint8_t *data_wr, size_t size);

void inline delay(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}