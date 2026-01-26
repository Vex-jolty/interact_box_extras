project(TriviaGame VERSION 1.0.0)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/questions.json)
	set(QUESTIONS_FILE questions.json)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${QUESTIONS_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else()
	set(QUESTIONS_FILE questions.example.json)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${QUESTIONS_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/icon/trivia_game_icon.ico DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/icon)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/app.manifest DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(TARGET_FILES trivia_game.cpp)

if (WIN32)
	set(OUT_RESOURCES_RC
		${CMAKE_CURRENT_BINARY_DIR}/resources.rc
	)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/resources.rc.in
		${OUT_RESOURCES_RC}
		@ONLY
	)
	list(APPEND TARGET_FILES ${OUT_RESOURCES_RC})
endif()

add_executable(
	trivia_game ${TARGET_FILES}
	
)

link_to_wxWidgets(trivia_game)

target_link_libraries(trivia_game PRIVATE extras_config)

target_link_libraries(trivia_game PRIVATE file_helper json_helper process_helper string_helper errors)

add_version_resource(trivia_game)