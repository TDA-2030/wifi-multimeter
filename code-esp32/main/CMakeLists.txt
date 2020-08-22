idf_component_register(SRCS "main.c" "web_server.c" "speech.c" "file_manage.c"
                    INCLUDE_DIRS "include"
                    )

set(AUDIO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../speech")
if(EXISTS ${AUDIO_DIR}/voice)
    execute_process(COMMAND bash ${AUDIO_DIR}/is_update.sh voice ../build
                    WORKING_DIRECTORY "${AUDIO_DIR}"
                    RESULT_VARIABLE AUDIO_IS_UPDATED
                    )
    if(AUDIO_IS_UPDATED EQUAL 27)
        spiffs_create_partition_image(audio ${AUDIO_DIR}/voice FLASH_IN_PROJECT)
    endif()
else()
    message(FATAL_ERROR "${AUDIO_DIR}/dist doesn't exit. ")
endif()