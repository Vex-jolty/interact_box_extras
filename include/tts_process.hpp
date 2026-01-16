#pragma once

#include "processes.hpp"
#include "exported.hpp"
#include "tts_process/resources.h"
#include <sapi.h>
#include <sphelper.h>
#include <string>
#include <vector>
#include <string_view>
#include "string_helper.hpp"
#include <boost/algorithm/algorithm.hpp>

void loadFileInResource(int name, std::string type, DWORD &size, const char *&data);
void replaceBannedWords(
	std::string &input,
	std::vector<std::string> bannedWords,
	std::string replacement
);