idf_component_register(SRCS "main.c" "web_server.c" "speech.c"
                    INCLUDE_DIRS "include"
                    )

set(AUDIO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../speech")
if(EXISTS ${AUDIO_DIR}/voice)

    spiffs_create_partition_image(audio ${AUDIO_DIR}/voice FLASH_IN_PROJECT)
else()
    message(FATAL_ERROR "${AUDIO_DIR}/dist doesn't exit. ")
endif()