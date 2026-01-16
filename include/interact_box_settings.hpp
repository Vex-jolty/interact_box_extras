#pragma once
#include "processes.hpp"
#include "exported.hpp"
#include <wx/wx.h>
#include "interact_box_settings/widgets/widgets_exported.hpp"
#include "interact_box_settings/resources.h"
#include <json/json_features.h>
#include <variant>

enum { SAVE_BUTTON = 3000, CLOSE_BUTTON = 3001 };

class InteractBoxSettings : public wxApp {
	public:
		virtual bool OnInit();
		int OnExit();
};

class MyFrame : public wxFrame {
	public:
		MyFrame();

	private:
		void parseSettings(wxPanel *panel, wxBoxSizer *sizer);
		void OnSave(wxCommandEvent &event);
		void OnExit(wxCommandEvent &event);
		void OnClose(wxCloseEvent &event);
		void OnChange(wxCommandEvent &event);
		void promptUserIfModified();
		int getSettingPositionPriorityByType(const Json::Value &setting);
		int getSettingPositionPriorityByName(
			const std::string &key,
			const std::unordered_map<std::string, int> &namePriorities
		);
		std::vector<std::pair<std::string, Json::Value>> sortSettings(Json::Value &initialSettings);

		bool _hasSaved = false;
		bool _isModified = false;
		std::vector<std::pair<std::string, Json::Value>> _sortedSettings;
};

enum {
	ID_SAVE = 1,
	ID_CHANGE = 2,
};
