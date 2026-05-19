# This script is a replacement for Windows' resource functionality
# We're essentially converting files that would be resources on Windows to hex arrays, so that Linux can use them

if(CMAKE_ARGC LESS 6)
	message(FATAL_ERROR "Usage: cmake -P file_to_hex.cmake <input_file> <output_file>")
endif()

set(INPUT_FILE "${CMAKE_ARGV3}")
set(OUTPUT_FILE "${CMAKE_ARGV4}")
set(VAR_NAME "${CMAKE_ARGV5}")

file(READ "${INPUT_FILE}" HEX_CONTENT HEX)

# This ensures the output is valid hex
string(REGEX MATCHALL ".." HEX_TOKENS "${HEX_CONTENT}")
string(REGEX REPLACE ";(..)" ", 0x\\1" FORMATTED_HEX "0x${HEX_TOKENS}")

string(LENGTH "${HEX_CONTENT}" HEX_LENGTH)
math(EXPR BYTE_LENGTH "${HEX_LENGTH} / 2")

set(FILE_PAYLOAD "unsigned char ${VAR_NAME}[] = {\n  ${FORMATTED_HEX}\n};\n")
string(CONCAT FILE_PAYLOAD "${FILE_PAYLOAD}" "unsigned int ${VAR_NAME}_len = ${BYTE_LENGTH};\n")

file(WRITE "${OUTPUT_FILE}" "${FILE_PAYLOAD}")