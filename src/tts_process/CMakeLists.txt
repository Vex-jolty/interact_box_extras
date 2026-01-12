project(TtsProcess VERSION 1.0.0)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/banned_words.txt)
  set(BANNED_WORDS_FILE banned_words.txt)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${QUESTIONS_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else()
  set(BANNED_WORDS_FILE banned_words.example.txt)
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${QUESTIONS_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

set(OUT_RESOURCES_RC
  ${CMAKE_CURRENT_BINARY_DIR}/resources.rc
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/resources.rc.in
  ${OUT_RESOURCES_RC}
  @ONLY
)

add_executable(
  tts_process tts_process.cpp
  resources.rc
)

target_include_directories(tts_process PRIVATE ${CMAKE_SOURCE_DIR}/interact_box_libs/string_helper)

target_link_libraries(tts_process PRIVATE platform_config extras_config uuid sapi string_helper errors file_helper)

add_version_resource(tts_process)