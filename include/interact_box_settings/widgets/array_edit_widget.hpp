#pragma once
#include "widgets.hpp"
#include "../structs/setting.hpp"

class ArrayEditWidget : public wxTextCtrl {
public:
	ArrayEditWidget(wxWindow* parent, wxWindowID id, std::string name, Json::Value& jsonSettings) : _name(name), _jsonSettings(jsonSettings) {
		_arrayValue = _jsonSettings[_name];
		Create(parent, id);
		_setTextValue();
		Bind(wxEVT_TEXT, &onTextChange, this, id);
	}

private:
	std::string _name;
	Json::Value& _jsonSettings;
	Json::Value _arrayValue;
	Structs::Setting* _setting;
	void onTextChange(wxCommandEvent& event);
	void _setTextValue();
};