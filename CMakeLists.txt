cmake_minimum_required(VERSION 3.10)

project(interact_box_extras VERSION 1.2.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	if (NOT MINGW)
		message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} is not a currently supported compiler")
	endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")

else()
	message(FATAL_ERROR "${CMAKE_SYSTEM_NAME} is not a currently supported operating system")
endif()

if (NOT BUILDING_FULL_PACKAGE)
	add_subdirectory(${CMAKE_SOURCE_DIR}/interact_box_libs)
endif()

set(TARGET_LINUX_GUI "qt" CACHE STRING "The GUI to use on Linux")

set_property(CACHE TARGET_LINUX_GUI PROPERTY STRINGS gtk3 qt)

set(TARGET_WINDOWS_VERSION "win98" CACHE STRING "The version of Windows to target (win98, winme, win2k, winxp, vista, win7, win8, win10, win11)")

set_property(CACHE TARGET_WINDOWS_VERSION PROPERTY STRINGS
	win98 winme win2k winxp vista win7 win8 win10 win11
)

set(VERSIONS_THAT_REQUIRE_PTHREAD9X win98 winme win2k winxp)
set(VERSIONS_THAT_REQUIRE_MSVCRT40 win98 winme win2k winxp)
set(VERSIONS_THAT_REQUIRE_WRAPPERS win98 winme)

if(NOT DEFINED TARGET_WINDOWS_VERSION)
	set(TARGET_WINDOWS_VERSION "win98"
		CACHE STRING "Target Windows version"
	)
endif()

if (NOT DEFINED TARGET_LINUX_GUI)
	set(TARGET_LINUX_GUI "qt"
		CACHE STRING "Target Linux GUI"
	)
endif()

if (WIN32 AND TARGET_WINDOWS_VERSION IN_LIST VERSIONS_THAT_REQUIRE_WRAPPERS)
	set(WX_VERSION "2.9")
else()
	set(WX_VERSION "3.2")
endif()

# This block is VERY SPECIFIC to the maintainer's setup
#add_definitions(wxWidgets_ROOT_DIR ${CMAKE_PREFIX_PATH})
if(WIN32)
	if(CMAKE_C_COMPILER_TARGET)
			set(WX_COMPILER_TRIPLET ${CMAKE_C_COMPILER_TARGET})
	elseif(MINGW)
		if(TARGET_WINDOWS_VERSION IN_LIST VERSIONS_THAT_REQUIRE_MSVCRT40)
			set(WX_COMPILER_TRIPLET i686-w64-mingw32)
		else()
			set(WX_COMPILER_TRIPLET x86_64-w64-mingw32)
		endif()
	else()
			message(FATAL_ERROR "Cannot determine wxWidgets compiler triplet")
	endif()
	set(WXWIDGETS_INCLUDE_DIR "${CMAKE_PREFIX_PATH}/include/wx-${WX_VERSION}")
	set(WXWIDGETS_SETUP_DIR "${CMAKE_PREFIX_PATH}/lib/wx/include/${WX_COMPILER_TRIPLET}-msw-unicode-static-${WX_VERSION}")
endif()

add_library(mingw_64_compat STATIC src/compat/mingw_64_compat.cpp)

function(link_to_wxWidgets target)
	if(WIN32)
		target_include_directories(${target} PRIVATE ${WXWIDGETS_SETUP_DIR} ${WXWIDGETS_INCLUDE_DIR})
		set_windows_version_definitions(${target} ${TARGET_WINDOWS_VERSION})
		if (TARGET_WINDOWS_VERSION IN_LIST VERSIONS_THAT_REQUIRE_PTHREAD9X)
			target_link_libraries(${target} INTERFACE pthread-9x)
			target_compile_options(${target} INTERFACE -m32 -march=i486)
		else()
			target_link_libraries(${target} INTERFACE winpthread)
		endif()

		if (TARGET_WINDOWS_VERSION IN_LIST VERSIONS_THAT_REQUIRE_WRAPPERS)
			target_link_libraries(${target} INTERFACE corkel32 corusr cornt)
		endif()
		
		target_compile_definitions(${target} PRIVATE
			FD_SETSIZE=1024
			__GNUWIN32__
			STRICT
			HAVE_W32API_H
			__WXMSW__
			__WINDOWS__
			UNICODE
			_UNICODE
		)
		set(WX_LIBS_TO_LINK 
			wx_mswu_core-${WX_VERSION}-${WX_COMPILER_TRIPLET}
			wx_baseu-${WX_VERSION}-${WX_COMPILER_TRIPLET}
			wx_baseu_net-${WX_VERSION}-${WX_COMPILER_TRIPLET}
		)
		if (${target} STREQUAL "malware_date_settings")
			list(APPEND WX_LIBS_TO_LINK wx_mswu_adv-${WX_VERSION}-${WX_COMPILER_TRIPLET})
		elseif (${target} STREQUAL "trivia_game")
			list(APPEND WX_LIBS_TO_LINK
			wx_mswu_stc-${WX_VERSION}-${WX_COMPILER_TRIPLET}
			wx_mswu_qa-${WX_VERSION}-${WX_COMPILER_TRIPLET}
			wx_mswu_ribbon-${WX_VERSION}-${WX_COMPILER_TRIPLET}
			wx_mswu_adv-${WX_VERSION}-${WX_COMPILER_TRIPLET}
		)
		endif()
		list(APPEND WX_LIBS_TO_LINK wxscintilla-${WX_VERSION}-${WX_COMPILER_TRIPLET})
		target_link_libraries(${target} PRIVATE
			${WX_LIBS_TO_LINK}
			shell32
			shlwapi
			uuid
			ole32
			oleaut32
			oleacc
			jpeg
			png
			# IMPORTANT: there needs to be a static libz.a for this to work!
			# Because of this, a custom PKGBUILD might be necessary
			# I'm considering just creating a mingw-w64-zlib-static package for the AUR to account for this
			z
			tiff
			kernel32
			atomic
			mingw32
			advapi32
			mingwex
			user32
			expat
			pcre2-16
			uxtheme
			glu32
			regex
			wsock32 comctl32 shell32 advapi32 gdi32 winmm comdlg32 version ws2_32 winhttp
			rpcrt4
			winspool
		)
		if (TARGET_WINDOWS_VERSION IN_LIST VERSIONS_THAT_REQUIRE_MSVCRT40)
			target_link_libraries(${target} PRIVATE
				msvcrt40
			)
		else()
			target_link_libraries(${target} PRIVATE
				msvcrt
			)
		endif()
	else()
		execute_process(
			COMMAND wx-config --cxxflags --toolkit=${TARGET_LINUX_GUI} --static=yes
			OUTPUT_VARIABLE WX_CXX_FLAGS
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

		execute_process(
			COMMAND wx-config --libs --toolkit=${TARGET_LINUX_GUI} --static=yes
			OUTPUT_VARIABLE WX_LD_FLAGS
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)

		separate_arguments(WX_LD_FLAGS)

		set(WX_VERSION "3.3")
		set(WX_LIBS_TO_LINK 
			${WX_LD_FLAGS}
			X11
		)
	endif()
	separate_arguments(WX_CXX_FLAGS)
	separate_arguments(WX_LIBS_TO_LINK)
	target_compile_options(${target} PRIVATE ${WX_CXX_FLAGS})
	target_link_libraries(${target} PRIVATE ${WX_LIBS_TO_LINK})
endfunction()

function(add_version_resource target)
	if(WIN32)
		set(INTERNAL_NAME "${target}.exe")
		set(ORIGINAL_FILENAME "${target}.exe")

		string(REPLACE
			"." "," VERSION_STRING ${CMAKE_PROJECT_VERSION}
		)

		set(OUT_RC
			${CMAKE_CURRENT_BINARY_DIR}/${target}_version.rc
		)

		configure_file(
			${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in
			${OUT_RC}
			@ONLY
		)

		target_sources(${target} PRIVATE ${OUT_RC})
	endif()
endfunction()

function(embed_file_as_header target input_file output_header variable_name output_var)
	get_filename_component(ABS_INPUT_FILE "${input_file}" ABSOLUTE)
	set(OUT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${output_header}.h")
	
	add_custom_command(
		OUTPUT ${OUT_HEADER}
		DEPENDS ${ABS_INPUT_FILE}
		COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/cmake/file_to_hex.cmake"
						"${ABS_INPUT_FILE}" "${OUT_HEADER}" "${variable_name}"
		COMMENT "Embedding asset: ${input_file} -> ${variable_name}"
	)
	set_property(SOURCE ${target} APPEND PROPERTY OBJECT_DEPENDS "${OUT_HEADER}")
	set(${output_var} ${OUT_HEADER} PARENT_SCOPE)
endfunction()

add_library(extras_config INTERFACE)

target_include_directories(extras_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/interact_box_libs/include)

target_compile_options(extras_config INTERFACE
	-fcf-protection=none
	-fpermissive
)

target_compile_options(extras_config INTERFACE
	$<$<CONFIG:Release>:-ffunction-sections -fdata-sections -s>
	$<$<CONFIG:Debug>:-g -gdwarf-4 -O0 -fno-omit-frame-pointer>
)

target_link_options(extras_config INTERFACE
	$<$<CONFIG:Release>:-Wl,--gc-sections>
	$<$<CONFIG:Debug>:-g>
)

target_link_options(extras_config INTERFACE
	$<$<PLATFORM_ID:Windows>:-Wl,--subsystem,windows>
)

target_link_libraries(extras_config INTERFACE mingw_64_compat)

if(WIN32)
	# For some reason, not including static versions of libgcc and libstdc++ suddenly started breaking wxWidget apps on runtime
	# This check should be compiler agnostic
	target_link_options(extras_config INTERFACE
		-static
		$<$<CXX_COMPILER_ID:GNU>:
			-static-libgcc
			-static-libstdc++
		>
		$<$<CXX_COMPILER_ID:Clang,AppleClang>:
			-static-libstdc++
		>
	)

	if(MSVC)
		set_property(TARGET extras_config PROPERTY
			MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
		)
	endif()
	set_windows_version_definitions(extras_config ${TARGET_WINDOWS_VERSION})
endif()

execute_process(
	COMMAND ${WX_CONFIG} --cxxflags
	OUTPUT_VARIABLE WX_CXX_FLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
	COMMAND ${WX_CONFIG} --libs
	OUTPUT_VARIABLE WX_LD_FLAGS
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(TARGETS_FOR_EXPORT
	interact_box_settings
	malware_date_settings
	message_box_process
	trivia_game
)

add_subdirectory(src/interact_box_settings)
add_subdirectory(src/malware_date_settings)
add_subdirectory(src/message_box_process)
add_subdirectory(src/trivia_game)
if (LINUX OR (WIN32 AND NOT TARGET_WINDOWS_VERSION IN_LIST VERSIONS_THAT_REQUIRE_WRAPPERS))
	add_subdirectory(src/tts_process)
	list(APPEND TARGETS_FOR_EXPORT tts_process)
endif()

install(
	TARGETS
		${TARGETS_FOR_EXPORT}
	EXPORT InteractBoxExtrasTargets
)

install(
	EXPORT InteractBoxExtrasTargets
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION lib/cmake/${PROJECT_NAME}
)

install(
	DIRECTORY include/
	DESTINATION include
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
	cmake/InteractBoxExtrasConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/InteractBoxExtrasConfig.cmake
	INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/InteractBoxExtrasConfig.cmake
	DESTINATION lib/cmake/${PROJECT_NAME}
)