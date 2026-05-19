project(tts_process VERSION 1.0.0)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/banned_words.txt)
	set(BANNED_WORDS_FILE banned_words.txt)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${BANNED_WORDS_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else()
	set(BANNED_WORDS_FILE banned_words.example.txt)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${BANNED_WORDS_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

set(TARGET_FILES ${PROJECT_NAME}.cpp)

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
else()
	embed_file_as_header(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${BANNED_WORDS_FILE} banned_words banned_words_txt GENERATED_BANNED_WORDS_H)
endif()

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/../../include/${PROJECT_NAME}/resources.h.in
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/resources.h
	@ONLY
)

add_executable(
	${PROJECT_NAME} ${TARGET_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/interact_box_libs/string_helper)

set(LIBRARIES_TO_LINK
	platform_config
	extras_config
)

if (WIN32)
	list(APPEND LIBRARIES_TO_LINK uuid sapi)
endif()

list(APPEND LIBRARIES_TO_LINK string_helper errors file_helper)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES_TO_LINK})

add_version_resource(${PROJECT_NAME})