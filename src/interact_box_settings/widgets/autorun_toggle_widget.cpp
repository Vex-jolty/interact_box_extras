#include "interact_box_settings/widgets/autorun_toggle_widget.hpp"

AutorunToggleWidget::AutorunToggleWidget(wxWindow* parent, wxWindowID id) {
#if WINVER > _WIN32_WINNT_NT4
	_regKeyToOpen = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	_interactBoxKeyName = L"InteractBox";
#else
	_regKeyToOpen = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	_interactBoxKeyName = "InteractBox";
#endif
	Create(parent, id, "Run Interact Box on logon");
	SetValue(getAutorunStatus());
	Bind(wxEVT_CHECKBOX, &OnChange, this, id);
}

bool AutorunToggleWidget::getAutorunStatus() {
	HKEY hKey;
	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
#if WINVER > _WIN32_WINNT_NT4
	long result = RegOpenKeyEx(HKEY_CURRENT_USER, _regKeyToOpen.c_str(), 0, KEY_QUERY_VALUE, &hKey);
#else
	long result = RegOpenKeyExA(HKEY_CURRENT_USER, _regKeyToOpen.c_str(), 0, KEY_QUERY_VALUE, &hKey);
#endif
	if (result != ERROR_SUCCESS) throw InteractBoxException(ErrorCodes::CannotOpenRegistryKey, _regKeyToOpen);

#if WINVER > _WIN32_WINNT_NT4
	result = RegQueryValueEx(hKey, _interactBoxKeyName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
#else
	result = RegQueryValueExA(hKey, _interactBoxKeyName.c_str(), 0, NULL, (LPBYTE)szBuffer, &dwBufferSize);
#endif
	return result == ERROR_SUCCESS;
}


bool AutorunToggleWidget::toggleAutorun(bool autorunIsEnabled) {
	try {
		HKEY hKey;
#if WINVER > _WIN32_WINNT_NT4
		long result = RegOpenKeyEx(HKEY_CURRENT_USER, _regKeyToOpen.c_str(), 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
#else
		long result = RegOpenKeyExA(HKEY_CURRENT_USER, _regKeyToOpen.c_str(), 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hKey);
#endif
		if (result != ERROR_SUCCESS) throw InteractBoxException(ErrorCodes::CannotOpenRegistryKey, _regKeyToOpen);
		if (autorunIsEnabled) {
#if WINVER > _WIN32_WINNT_NT4
			result = RegDeleteValue(hKey, _interactBoxKeyName.c_str());
#else
			result = RegDeleteValueA(hKey, _interactBoxKeyName.c_str());
#endif
			if (result != ERROR_SUCCESS) {
				RegCloseKey(hKey);
				throw InteractBoxException(ErrorCodes::CannotDeleteRegistryKey, _regKeyToOpen);
			}
			RegCloseKey(hKey);
			return true;
		}
#if WINVER > _WIN32_WINNT_NT4
		std::wstring val = FileHelper::getWorkingDirectory() + L"\\interact_box.exe";
		result = RegSetValueEx(hKey, _interactBoxKeyName.c_str(), 0, REG_SZ, (BYTE*)val.c_str(), (val.size() + 1) * sizeof(wchar_t));
#else
		std::string val = FileHelper::getWorkingDirectory() + "\\interact_box.exe";
		result = RegSetValueExA(hKey, _interactBoxKeyName.c_str(), 0, REG_SZ, (BYTE*)val.c_str(), (val.size() + 1) * sizeof(char));
#endif
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