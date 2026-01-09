#include "interact_box_settings/widgets/autorun_toggle_widget.hpp"

AutorunToggleWidget::AutorunToggleWidget(wxWindow* parent, wxWindowID id) {
	_regKeyToOpen = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	Create(parent, id, "Run Interact Box on logon");
	SetValue(getAutorunStatus());
	Bind(wxEVT_CHECKBOX, &OnChange, this, id);
}

bool AutorunToggleWidget::getAutorunStatus() {
	HKEY hKey;
	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	long result = RegOpenKeyEx(HKEY_CURRENT_USER, _regKeyToOpen.c_str(), 0, KEY_QUERY_VALUE, &hKey);
	if (result != ERROR_SUCCESS) throw InteractBoxException(ErrorCodes::CannotOpenRegistryKey, _regKeyToOpen);

	result = RegQueryValueEx(hKey, L"InteractBox", 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
	return result == ERROR_SUCCESS;
}


bool AutorunToggleWidget::toggleAutorun(bool autorunIsEnabled) {
	try {
		HKEY hKey;
		long result = RegOpenKeyEx(HKEY_CURRENT_USER, _regKeyToOpen.c_str(), 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
		if (result != ERROR_SUCCESS) throw InteractBoxException(ErrorCodes::CannotOpenRegistryKey, _regKeyToOpen);
		if (autorunIsEnabled) {
			result = RegDeleteValue(hKey, L"InteractBox");
			if (result != ERROR_SUCCESS) {
				RegCloseKey(hKey);
				throw InteractBoxException(ErrorCodes::CannotDeleteRegistryKey, _regKeyToOpen);
			}
			RegCloseKey(hKey);
			return true;
		}
		#if WINVER > _WIN32_WINNT_NT4
		std::wstring val = FileHelper::getWorkingDirectory() + L"\\interact_box.exe";
		#else
		std::string val = FileHelper::getWorkingDirectory() + "\\interact_box.exe";
		#endif
		result = RegSetValueEx(hKey, L"InteractBox", 0, REG_SZ, (BYTE*)val.c_str(), (val.size() + 1) * sizeof(wchar_t));
		if (result != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			throw InteractBoxException(ErrorCodes::CannotSetRegistryKey, _regKeyToOpen);
		}
		RegCloseKey(hKey);
		return true;
	} catch (InteractBoxException& e) {
		MessageBoxA(NULL, e.what().c_str(), "ERROR", MB_ICONERROR);
		return false;
	}
}

void AutorunToggleWidget::OnChange(wxCommandEvent& event) {
	toggleAutorun(!event.IsChecked());
	event.Skip();
}