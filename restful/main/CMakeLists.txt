idf_component_register(SRCS "esp_rest_main.c" 
                            "rest_server.c"
                    INCLUDE_DIRS ".")

if(CONFIG_EXAMPLE_WEB_DEPLOY_SF)
    set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../front/web-demo")
    if(EXISTS ${WEB_SRC_DIR}/dist)
        spiffs_create_partition_image(www ${WEB_SRC_DIR}/dist FLASH_IN_PROJECT)
    else()
        message(FATAL_ERROR "${WEB_SRC_DIR}/dist doesn't exit. Please run 'npm run build' in ${WEB_SRC_DIR}")
    endif()
endif()

if(CONFIG_EXAMPLE_WEB_DEPLOY_SD)
    set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../front/web-demo")
    if(EXISTS ${WEB_SRC_DIR}/dist)
        message(STATUS "Start make littlefs image and flash ")
        execute_process(COMMAND ../mklittlefs -c ${WEB_SRC_DIR}/dist -s 4194304 ../little_img.bin
                        COMMAND esptool.py -b 921600 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_size detect --flash_freq 80m 0x110000 ../fat_img.bin
                        WORKING_DIRECTORY .
                       )
    else()
        message(FATAL_ERROR "${WEB_SRC_DIR}/dist doesn't exit. Please run 'npm run build' in ${WEB_SRC_DIR}")
    endif()
endif()
