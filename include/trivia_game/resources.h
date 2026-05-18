#pragma once
#define IDR_JSON 101
#define IDI_ICON1 102
#ifdef __linux__
	#if __has_include("questions.h")
  	#include "questions.h"
	#else
		#include "questions_example.h"
	#endif
  #include "./icon.h"
#endif