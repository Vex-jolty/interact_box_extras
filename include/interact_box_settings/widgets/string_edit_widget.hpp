#pragma once
#include "widgets.hpp"
#include "../structs/setting.hpp"

class StringEditWidget : public wxTextCtrl {
	public:
		StringEditWidget(wxWindow *parent, wxWindowID id, std::string name, Json::Value &jsonSettings)
				: _name(name), _jsonSettings(jsonSettings) {
			/*if (_setting.name == "host") {
				wxIP
			}*/

			Create(parent, id, _jsonSettings[_name].asString());
			Bind(wxEVT_TEXT, &onTextChange, this, id);
		}

	private:
		std::string _name;
		Json::Value &_jsonSettings;
		void onTextChange(wxCommandEvent &event) {
			_jsonSettings[_name] = (std::string)event.GetString();
			event.Skip();
		}
};