project(message_box_process VERSION 1.2.1)

find_package(Boost REQUIRED CONFIG)

set(TARGET_FILES ${PROJECT_NAME}.cpp)

if (WIN32)
	set(OUT_RESOURCES_RC
		${CMAKE_CURRENT_SOURCE_DIR}/resources.rc
	)
	list(APPEND TARGET_FILES ${OUT_RESOURCES_RC})
else()
	embed_file_as_header(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/icon/linux.ico icon icon_${PROJECT_NAME}_ico GENERATED_ICON_H)
	list(APPEND TARGET_FILES ${GENERATED_ICON_H})
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

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	string_helper
)

add_version_resource(${PROJECT_NAME})