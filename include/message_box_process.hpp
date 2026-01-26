#pragma once
#include "processes.hpp"
#include "string_helper.hpp"
#include "message_box_process/resources.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <wx/wx.h>
#include <wx/log.h>
#include <wx/msgdlg.h>
#ifdef __linux__
	#include <wx/mstream.h>
#endif

#ifdef WIN32
std::vector<std::wstring> parseButtons(std::wstring buttonsString);
#else
std::vector<std::string> parseButtons(std::string buttonsString);
#endif

struct DefaultButtonOption {
#ifdef WIN32
		std::vector<std::wstring> validInputs;
		std::vector<std::wstring> resultingButtons;
#else
		std::vector<std::string> validInputs;
		std::vector<std::string> resultingButtons;
#endif
};

class CustomBox : public wxMessageDialog {
	public:
		CustomBox(
#ifdef WIN32
			std::wstring msg,
			std::wstring title,
			long typeCode,
			std::vector<std::wstring> buttons
#else
			std::string msg,
			std::string title,
			long typeCode,
			std::vector<std::string> buttons
#endif
		)
				: wxMessageDialog(
						NULL,
						msg,
						title,
						typeCode |
							(buttons.size() == 1		 ? wxOK
								 : buttons.size() == 2 ? wxYES_NO
																			 : wxYES_NO | wxCANCEL)
					),
					_buttons(buttons) {
			_handleButtons();
		}

	private:
		void _handleButtons();
#ifdef WIN32
		std::vector<std::wstring> _buttons;
#else
		std::vector<std::string> _buttons;
#endif
};

class MessageBoxApp : public wxApp {
	public:
		virtual bool OnInit();
};