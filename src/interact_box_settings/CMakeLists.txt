project(InteractBoxSettings VERSION 1.0.0)

add_executable(
  interact_box_settings interact_box_settings.cpp
  widgets/array_edit_widget.cpp
	widgets/autorun_toggle_widget.cpp
  widgets/bool_edit_widget.cpp
  widgets/int_edit_widget.cpp
  widgets/string_edit_widget.cpp
  widgets/logging_edit_widget.cpp
  resources.rc
)

set_windows_version_definitions(interact_box_settings ${TARGET_WINDOWS_VERSION})

target_link_libraries(
  interact_box_settings PRIVATE
  extras_config
  errors
  string_helper
  file_helper
  json_helper
  process_helper
  logging_level
)

link_to_wxWidgets(interact_box_settings)

add_version_resource(interact_box_settings)