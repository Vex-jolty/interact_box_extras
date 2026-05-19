project(trivia_game VERSION 1.2.1)

set(MAIN_QUESTIONS_FILE questions.json)
set(FALLBACK_QUESTIONS_FILE questions.example.json)

set(ICON_FILE icon/${PROJECT_NAME}_icon.ico)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${MAIN_QUESTIONS_FILE})
	set(QUESTIONS_FILE ${MAIN_QUESTIONS_FILE})
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${QUESTIONS_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else()
	set(QUESTIONS_FILE ${FALLBACK_QUESTIONS_FILE})
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${QUESTIONS_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/${ICON_FILE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/icon)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/app.manifest DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

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
	embed_file_as_header(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${QUESTIONS_FILE} questions questions_json GENERATED_QUESTIONS_H)
	embed_file_as_header(${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${ICON_FILE} icon icon_${PROJECT_NAME}_ico GENERATED_ICON_H)
	list(APPEND TARGET_FILES ${GENERATED_QUESTIONS_H} ${GENERATED_ICON_H})
endif()

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/../../include/${PROJECT_NAME}/resources.h.in
	${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_include/resources.h
	@ONLY
)

add_executable(
	${PROJECT_NAME} ${TARGET_FILES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

target_link_libraries(${PROJECT_NAME} PRIVATE platform_config extras_config)

link_to_wxWidgets(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE file_helper json_helper process_helper string_helper errors)

add_version_resource(${PROJECT_NAME})