#pragma once
#include "widgets.hpp"
#include "../structs/setting.hpp"
#include "logging_level.hpp"

class LoggingEditWidget : public wxChoice {
public:
	LoggingEditWidget(wxWindow* parent, int id, Json::Value& jsonSettings);
private:
	wxArrayString _choices;
	Json::Value& _jsonSettings;
	void onChoice(wxCommandEvent& event);
};