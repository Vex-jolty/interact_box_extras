#pragma once
#include "widgets.hpp"
#include "../structs/setting.hpp"

class BoolEditWidget : public wxCheckBox {
public:
	BoolEditWidget(
		wxWindow* parent,
		wxWindowID id,
		std::string name,
		Json::Value& jsonSettings
	) : _name(name), _jsonSettings(jsonSettings), _label(StringHelper::camelCaseToHuman(name, true)) {
		Create(parent, id, _label);
		SetValue(_jsonSettings[_name].asBool());
		Bind(wxEVT_CHECKBOX, &onChange, this, id);
	}
	

private:
	std::string _name;
	std::string _label;
	Json::Value& _jsonSettings;
	void onChange(wxCommandEvent& event) {
		_jsonSettings[_name] = event.IsChecked();
		event.Skip();
	}
};