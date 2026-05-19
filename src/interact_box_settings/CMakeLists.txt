project(interact_box_settings VERSION 1.2.2)

set(TARGET_FILES ${PROJECT_NAME}.cpp
	widgets/array_edit_widget.cpp
	widgets/autorun_toggle_widget.cpp
	widgets/bool_edit_widget.cpp
	widgets/int_edit_widget.cpp
	widgets/string_edit_widget.cpp
	widgets/logging_edit_widget.cpp
)

if (WIN32)
	set(OUT_RESOURCES_RC
		${CMAKE_CURRENT_SOURCE_DIR}/resources.rc
	)
	list(APPEND TARGET_FILES ${OUT_RESOURCES_RC})
else()
	embed_file_as_header(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/icon/config_icon-linux.ico icon icon_${PROJECT_NAME}_ico GENERATED_ICON_H)
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

set_windows_version_definitions(${PROJECT_NAME} ${TARGET_WINDOWS_VERSION})

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	extras_config
	errors
	string_helper
	file_helper
	json_helper
	process_helper
	logging_level
)

link_to_wxWidgets(${PROJECT_NAME})

add_version_resource(${PROJECT_NAME})