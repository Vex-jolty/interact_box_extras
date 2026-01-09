project(TriviaGame VERSION 1.0.0)

add_executable(
  trivia_game trivia_game.cpp
  resources.rc
)

link_to_wxWidgets(trivia_game)

target_link_libraries(trivia_game PRIVATE extras_config)

target_link_libraries(trivia_game PRIVATE file_helper json_helper process_helper string_helper errors)

add_version_resource(trivia_game)