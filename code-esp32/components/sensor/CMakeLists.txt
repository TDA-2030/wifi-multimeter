set(component_srcs "sensor.c" "ADS1015.c" "i2c_bus.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       REQUIRES driver esp_adc_cal
                       )

