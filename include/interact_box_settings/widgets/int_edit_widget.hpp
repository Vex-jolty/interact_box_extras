#pragma once
#include "widgets.hpp"
#include "../structs/setting.hpp"
#include <wx/valnum.h>

class IntEditWidget : public wxTextCtrl {
	public:
		IntEditWidget(wxWindow *parent, wxWindowID id, std::string name, Json::Value &jsonSettings)
				: _name(name), _jsonSettings(jsonSettings) {
			int settingVal = _jsonSettings[_name].asInt();
			wxIntegerValidator<int> val(&settingVal, wxNUM_VAL_DEFAULT);
			Create(parent, id, std::to_string(settingVal), wxDefaultPosition, wxDefaultSize, 0, val);
			Bind(wxEVT_TEXT, &onTextChange, this, id);
		}

	private:
		std::string _name;
		Json::Value &_jsonSettings;
		wxIntegerValidator<int> _val;
		void onTextChange(wxCommandEvent &event) {
			_jsonSettings[_name] = std::stoi(((std::string)event.GetString()).c_str());
			event.Skip();
		}
};