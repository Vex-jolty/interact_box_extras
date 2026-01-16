#include "interact_box_settings.hpp"

#define ICON_ID 1001

using namespace std;
using namespace Structs;

Json::Value jsonSettings;

string filePath = "C:\\WINDOWS\\interact_box_config.json";
#if WINVER > _WIN32_WINNT_NT4
string workingDirectory = FileHelper::getWorkingDirectoryAsString();
#else
string workingDirectory = FileHelper::getWorkingDirectory();
#endif
string interactBoxPath = "interact_box.exe";

bool isNumber(string input) {
	for (char &c : input) {
		if (!isdigit(c))
			return false;
	}
	return true;
}

int MyFrame::getSettingPositionPriorityByType(const Json::Value &setting) {
	if (setting.isString())
		return 1;
	if (setting.isInt64() || setting.isUInt64())
		return 2;
	if (setting.isArray())
		return 3;
	if (setting.isBool())
		return 4;
	return 5;
}

int MyFrame::getSettingPositionPriorityByName(
	const string &key,
	const std::unordered_map<std::string, int> &namePriorities
) {
	auto it = namePriorities.find(key);
	if (it != namePriorities.end())
		return it->second;
	return namePriorities.size() + 1;
}

vector<pair<string, Json::Value>> MyFrame::sortSettings(Json::Value &initialSettings) {
	unordered_map<std::string, int> namePriorities = {{"host", 1}, {"port", 2}};

	std::vector<std::pair<std::string, Json::Value>> entries;

	for (const auto &key : initialSettings.getMemberNames()) {
		entries.emplace_back(key, initialSettings[key]);
	}

	std::sort(entries.begin(), entries.end(), [&namePriorities, this](const auto &a, const auto &b) {
		int aNamePri = getSettingPositionPriorityByName(a.first, namePriorities);
		int bNamePri = getSettingPositionPriorityByName(b.first, namePriorities);
		if (aNamePri != bNamePri)
			return aNamePri < bNamePri;

		int aTypePri = getSettingPositionPriorityByType(a.second);
		int bTypePri = getSettingPositionPriorityByType(b.second);
		if (aTypePri != bTypePri)
			return aTypePri < bTypePri;

		return a.first < b.first; // Fallback: alphabetical
	});

	return entries;
}

/** cSpell:disable */

void saveSettings() { FileHelper::writeToFile(filePath, jsonSettings.toStyledString()); }

bool askToSave() {
	int response = wxMessageBox(
		"Some settings were changed, but the changes were not saved. Would you like to save them now?",
		"Interact Box XP Settings", wxICON_WARNING | wxYES_NO
	);
	return response == wxYES;
}

void restartInteractBox() {
	int response = wxMessageBox(
		"In order to apply the settings, Interact Box must be restarted. Would you like to restart it "
		"now?",
		"Interact Box XP Settings", wxICON_WARNING | wxYES_NO
	);
	if (response != wxYES)
		return;
	ProcessHelper::killProcess(interactBoxPath);
	ShellExecuteA(NULL, "open", interactBoxPath.c_str(), NULL, workingDirectory.c_str(), SW_SHOW);
}

void handleError(string errorMessage) {
	wxMessageBox(errorMessage, "INTERACT BOX XP SETTINGS ERROR", wxICON_ERROR);
}

bool InteractBoxSettings::OnInit() {
	MyFrame *frame = new MyFrame();
	SetTopWindow(frame);
	frame->Show(true);
	return true;
}

int InteractBoxSettings::OnExit() { return 0; }

MyFrame::MyFrame()
		: wxFrame(NULL, wxID_ANY, "Interact Box Settings", wxDefaultPosition, wxSize(400, 600)) {
	HICON iconHandle = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wxIcon icon;
	icon.CreateFromHICON(iconHandle);
	SetIcon(icon);
	DestroyIcon(iconHandle);
	vector<ArrayEditWidget *> arraySettings;
	auto parentPanel = new wxPanel(this, wxID_ANY);
	auto parentSizer = new wxBoxSizer(wxVERTICAL);

	auto scrollPanel =
		new wxScrolledWindow(parentPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
	scrollPanel->SetScrollRate(5, 10);
	auto scrollSizer = new wxBoxSizer(wxVERTICAL);

	try {
		string settingsFile = FileHelper::readFileAsString(filePath);
		jsonSettings = JsonHelper::parseJsonString(settingsFile);
		if (jsonSettings.empty()) {
			throw InteractBoxException(ErrorCodes::CannotReadFile, settingsFile);
		}
		_sortedSettings = sortSettings(jsonSettings);
	} catch (InteractBoxException &e) {
		handleError(e.what());
		abort();
	}
	MyFrame::parseSettings(scrollPanel, scrollSizer);
	scrollPanel->SetSizer(scrollSizer);
	scrollPanel->FitInside();

	auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
	auto buttonsPanel = new wxPanel(parentPanel, wxID_ANY);

	wxButton *saveButton = new wxButton(buttonsPanel, SAVE_BUTTON, "Save");
	wxButton *closeButton = new wxButton(buttonsPanel, CLOSE_BUTTON, "Close");
	buttonsSizer->Add(saveButton, 1, wxALL, 5);
	buttonsSizer->Add(closeButton, 1, wxALL, 5);
	buttonsPanel->SetSizerAndFit(buttonsSizer);

	parentSizer->Add(scrollPanel, 1, wxEXPAND, 1);
	parentSizer->Add(buttonsPanel, 0, wxALIGN_CENTER, 1);
	;

	parentPanel->SetSizer(parentSizer);
	parentSizer->Fit(parentPanel);
	parentPanel->Show(true);
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_SAVE, "&Save...\tCtrl-S", "Save the current settings");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("No changes so far");
	Bind(wxEVT_MENU, &MyFrame::OnSave, this, ID_SAVE);
	Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
	Bind(wxEVT_BUTTON, &MyFrame::OnSave, this, SAVE_BUTTON);
	Bind(wxEVT_BUTTON, &MyFrame::OnExit, this, CLOSE_BUTTON);
	Bind(wxEVT_CLOSE_WINDOW, &MyFrame::OnClose, this);
}

void MyFrame::parseSettings(wxPanel *panel, wxBoxSizer *sizer) {
	int idCounter = 50;

	// Placing the autorun toggle widget on top of all the others for ease of access
	auto autorunWidget = new AutorunToggleWidget(panel, idCounter);
	sizer->Add(autorunWidget, 1, wxEXPAND, 10);

	// All other widgets get placed in this for loop
	for (auto [name, val] : _sortedSettings) {
		int id = idCounter++;
		Json::Value value = jsonSettings[name];

		// Yes, this is terrible. It sucks to have so many else if statements, and nested if statements,
		// but it's the best I could do.
		if (value.isBool()) {
			auto item = new BoolEditWidget(panel, id, name, jsonSettings);
			sizer->Add(item, 1, wxEXPAND, 10);
			Bind(wxEVT_CHECKBOX, &MyFrame::OnChange, this, id);
		} else if (name == "loggingLevel") {
			sizer->Add(new wxStaticText(panel, id * 17, StringHelper::camelCaseToHuman(name, true)));
			auto item = new LoggingEditWidget(panel, id, jsonSettings);
			sizer->Add(item, 1, wxEXPAND, 10);
			Bind(wxEVT_CHOICE, &MyFrame::OnChange, this, id);
		} else if (value.isInt()) {
			sizer->Add(new wxStaticText(panel, id * 100, StringHelper::camelCaseToHuman(name, true)));
			auto item = new IntEditWidget(panel, id, name, jsonSettings);
			sizer->Add(item, 1, wxEXPAND, 10);
			Bind(wxEVT_TEXT, &MyFrame::OnChange, this, id);
		} else if (value.isString()) {
			if (name.ends_with("Dir")) {
				sizer->Add(
					new wxStaticText(panel, id * 100, StringHelper::camelCaseToHuman(name + "ectory", true))
				);
				auto item = new DirectoryEditWidget(panel, id, name, jsonSettings);
				sizer->Add(item, 1, wxEXPAND, 10);
				int buttonId = id * 13;
				Bind(wxEVT_BUTTON, &MyFrame::OnChange, this, buttonId);
			} else {
				sizer->Add(new wxStaticText(panel, id * 100, StringHelper::camelCaseToHuman(name, true)));
				auto item = new StringEditWidget(panel, id, name, jsonSettings);
				sizer->Add(item, 1, wxEXPAND, 10);
				Bind(wxEVT_TEXT, &MyFrame::OnChange, this, id);
			}
		} else if (value.isArray()) {
			sizer->Add(new wxStaticText(panel, id * 100, StringHelper::camelCaseToHuman(name, true)));
			auto item = new ArrayEditWidget(panel, id, name, jsonSettings);
			sizer->Add(item, 1, wxEXPAND, 10);
			Bind(wxEVT_TEXT, &MyFrame::OnChange, this, id);
		}
	}
}

void MyFrame::OnExit(wxCommandEvent &event) { Close(false); }

void MyFrame::OnClose(wxCloseEvent &event) {
	event.SetCanVeto(true);
	event.Veto(true);
	try {
		promptUserIfModified();
	} catch (exception &e) {
		handleError(e.what());
	}
	Destroy();
}

void MyFrame::OnSave(wxCommandEvent &event) {
	try {
		saveSettings();
	} catch (exception &e) {
		handleError(e.what());
	}
	SetStatusText("Saved!");
	_hasSaved = true;
}

void MyFrame::OnChange(wxCommandEvent &event) {
	_isModified = true;
	SetStatusText("Some settings have been changed!");
}

void MyFrame::promptUserIfModified() {
	if (!_isModified)
		return;
	if (_hasSaved) {
		restartInteractBox();
		return;
	}
	bool shouldSave = askToSave();
	if (!shouldSave)
		return;
	saveSettings();
	restartInteractBox();
}

wxIMPLEMENT_APP(InteractBoxSettings);