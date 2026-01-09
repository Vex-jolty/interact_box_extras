#pragma once
#include "widgets.hpp"
#include "../structs/setting.hpp"
#include <regex>

class DirectoryEditWidget : public wxPanel {
public:
	DirectoryEditWidget(wxWindow* parent, wxWindowID id, std::string name, Json::Value& jsonSettings) : _name(name), _jsonSettings(jsonSettings) {
		std::regex isFullPathPattern(R"([A-Z]:\\.*)");
		Create(parent, id);
		bool isFullPath = std::regex_match(_jsonSettings[_name].asString(), isFullPathPattern);
		if (!isFullPath) {
			#if WINVER > _WIN32_WINNT_NT4
			_jsonSettings[_name] = FileHelper::getWorkingDirectoryAsString() + "\\" + _jsonSettings[_name].asString();
			#else
			_jsonSettings[_name] = FileHelper::getWorkingDirectory() + "\\" + _jsonSettings[_name].asString();
			#endif
		}
		auto sizer = new wxBoxSizer(wxHORIZONTAL);
		int textControlId = id * 5;
		_textControl = new wxTextCtrl(this, textControlId, _jsonSettings[_name].asString());
		_textControl->Disable();
		sizer->Add(_textControl, 2);
		int buttonId = id * 13;
		_button = new wxButton(this, buttonId, "Change");
		sizer->Add(_button, 0);
		int dirDialogId = id * 17;
		_dirDialog = new wxDirDialog(parent, StringHelper::camelCaseToHuman(_name + "ectory"), _jsonSettings[_name].asString());
		SetSizerAndFit(sizer);
		Show();
		Bind(wxEVT_BUTTON, &onButtonClick, this, buttonId);
		Bind(wxEVT_TEXT, &onTextChange, this, id);
	}


private:
	std::string _name;
	Json::Value& _jsonSettings;
	wxTextCtrl* _textControl;
	wxButton* _button;
	wxDirDialog* _dirDialog;
	void onTextChange(wxCommandEvent& event) {
		_jsonSettings[_name] = (std::string)event.GetString();
		event.Skip();
	}
	void onButtonClick(wxCommandEvent& event) {
		int result = _dirDialog->ShowModal();
		if (result == wxID_OK) {
			#if WINVER > _WIN32_WINNT_NT4
			std::string path = _dirDialog->GetPath().utf8_string();
			#else
			std::string path = _dirDialog->GetPath().ToStdString();
			#endif
			_textControl->SetValue(path);
			_jsonSettings[_name] = path;
		}
		event.Skip();
	}
};