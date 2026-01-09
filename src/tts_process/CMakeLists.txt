project(TtsProcess VERSION 1.0.0)

add_executable(
  tts_process tts_process.cpp
  resources.rc
  version.rc
)

target_include_directories(tts_process PRIVATE ${CMAKE_SOURCE_DIR}/interact_box_libs/string_helper)

target_link_libraries(tts_process PRIVATE ${BASE_LIBS} kernel32 mingw32 gcc gcc_eh stdc++ shell32 advapi32 mingwex ole32 uuid sapi -static)
target_link_libraries(tts_process PRIVATE string_helper errors file_helper)

add_version_resource(tts_process)