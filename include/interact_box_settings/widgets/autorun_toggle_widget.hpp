#pragma once
#include "widgets.hpp"
#include "../structs/setting.hpp"

class AutorunToggleWidget : public wxCheckBox {
	public:
		AutorunToggleWidget(wxWindow *parent, wxWindowID id);
		bool getAutorunStatus();
		bool toggleAutorun(bool autorunIsEnabled);

	private:
		void OnChange(wxCommandEvent &event);
#if WINVER > _WIN32_WINNT_NT4
		std::wstring _regKeyToOpen;
		std::wstring _interactBoxKeyName;
#else
		std::string _regKeyToOpen;
		std::string _interactBoxKeyName;
#endif
};