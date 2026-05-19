project(message_box_process VERSION 1.0.0)

find_package(Boost REQUIRED CONFIG)

add_executable(
	${PROJECT_NAME} message_box_process.cpp
	resources.rc
)

target_link_libraries(${PROJECT_NAME} PRIVATE extras_config)

link_to_wxWidgets(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	string_helper
)

add_version_resource(${PROJECT_NAME})