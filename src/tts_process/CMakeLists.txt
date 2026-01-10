project(TtsProcess VERSION 1.0.0)

add_executable(
  tts_process tts_process.cpp
  resources.rc
)

target_include_directories(tts_process PRIVATE ${CMAKE_SOURCE_DIR}/interact_box_libs/string_helper)

target_link_libraries(tts_process PRIVATE platform_config extras_config uuid sapi string_helper errors file_helper)

add_version_resource(tts_process)