project(MessageBoxCreator VERSION 1.0.0)

find_package(Boost REQUIRED CONFIG)

add_executable(
	message_box_process message_box_process.cpp
	resources.rc
)

target_link_libraries(message_box_process PRIVATE extras_config)

link_to_wxWidgets(message_box_process)

target_link_libraries(message_box_process
	PRIVATE
	string_helper
)

add_version_resource(message_box_process)