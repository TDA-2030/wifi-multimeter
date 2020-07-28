idf_component_register(SRCS "main.c" "web_server.c"
                    INCLUDE_DIRS "include"
                    )

set(AUDIO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../speech/audio_file")
if(EXISTS ${AUDIO_DIR}/audio_file_acc)
    spiffs_create_partition_image(audio ${AUDIO_DIR}/audio_file_acc FLASH_IN_PROJECT)
else()
    message(FATAL_ERROR "${AUDIO_DIR}/dist doesn't exit. ")
endif()