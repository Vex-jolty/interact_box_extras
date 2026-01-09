project(TriviaGame VERSION 1.0.0)

add_executable(
  trivia_game trivia_game.cpp
  resources.rc
)

#target_include_directories(trivia_game PRIVATE /usr/i686-w64-mingw32/lib/wx/include/i686-w64-mingw32-msw-unicode-static-3.2)
#target_include_directories(trivia_game PRIVATE /usr/i686-w64-mingw32/include/wx-3.2)
#target_include_directories(trivia_game PRIVATE ${CMAKE_SOURCE_DIR}/shared/file_helper
#target_include_directories(trivia_game PRIVATE ${CMAKE_SOURCE_DIR}/shared/json_helper)
#target_include_directories(trivia_game PRIVATE ${CMAKE_SOURCE_DIR}/shared/process_helper)
#target_include_directories(trivia_game PRIVATE ${CMAKE_SOURCE_DIR}/shared/errors)

#target_link_directories(trivia_game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)

link_to_wxWidgets(trivia_game)

target_link_libraries(trivia_game PRIVATE extras_config)

target_link_libraries(trivia_game PRIVATE file_helper json_helper process_helper string_helper errors)

add_version_resource(trivia_game)