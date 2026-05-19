#pragma once

#include "processes.hpp"
#include "exported.hpp"
#include "tts_process/resources.h"
#ifdef WIN32
	#include <sapi.h>
	#include <sphelper.h>
#endif
#include <string>
#include <vector>
#include <string_view>
#include "string_helper.hpp"
#include <boost/algorithm/algorithm.hpp>

#ifdef WIN32
void loadFileInResource(int name, std::string type, DWORD& size, const char*& data);
#else
std::vector<std::string> loadBannedWords();
#endif
void replaceBannedWords(
	std::string& input,
	std::vector<std::string> bannedWords,
	std::string replacement
);