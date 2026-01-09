#pragma once
#include "widgets.hpp"
#include "../structs/setting.hpp"

class AutorunToggleWidget : public wxCheckBox {
public:
	AutorunToggleWidget(
		wxWindow* parent,
		wxWindowID id
	);
	bool getAutorunStatus();
	bool toggleAutorun(bool autorunIsEnabled);
private:
	void OnChange(wxCommandEvent& event);
	std::wstring _regKeyToOpen;
};