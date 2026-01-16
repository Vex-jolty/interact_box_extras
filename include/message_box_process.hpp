#pragma once
#include "processes.hpp"
#include "string_helper.hpp"
#include "message_box_process/resources.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <wx/wx.h>
#include <wx/log.h>
#include <wx/msgdlg.h>

std::vector<std::wstring> parseButtons(std::wstring buttonsString);

struct DefaultButtonOption {
	std::vector<std::wstring> validInputs;
	std::vector<std::wstring> resultingButtons;
};

class CustomBox : public wxMessageDialog {
	public:
		CustomBox(
			std::wstring msg,
			std::wstring title,
			long typeCode,
			std::vector<std::wstring> buttons
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
		std::vector<std::wstring> _buttons;
};

class MessageBoxApp : public wxApp {
	public:
		virtual bool OnInit();
};