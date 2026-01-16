#include "message_box_process.hpp"

using namespace std;

map<wstring, wstring> parsedArgs;
vector<wstring> buttonsToCapitalize = {L"OK",	 L"Help",		L"Abort",		 L"Retry", L"Ignore",
																			 L"Try", L"Cancel", L"Continue", L"Yes",	 L"No"};
vector<DefaultButtonOption> defaultOptions = {
	DefaultButtonOption(
		{
			L"abort/retry/ignore",
			L"ari",
			L"a/r/i",
			L"a r i",
			L"abort retry ignore"
			L"a;r;i",
			L"abort;retry;ignore",
		},
		{L"Abort", L"Retry", L"Ignore"}
	),
	DefaultButtonOption(
		{
			L"cancel/try/continue",
			L"ctc",
			L"c/t/c",
			L"c t c",
			L"cancel try continue",
			L"c;t;c",
			L"cancel;try;continue",
		},
		{L"Cancel", L"Try", L"Continue"}
	),
	DefaultButtonOption({L"help", L"h"}, {L"Help"}),
	DefaultButtonOption(
		{L"ok/cancel", L"oc", L"kc", L"o/c", L"k/c", L"o c", L"k c", L"cancel", L"ok cancel",
		 L"ok;cancel", L"k;c", L"o;c"},
		{L"OK", L"Cancel"}
	),
	DefaultButtonOption(
		{L"retry/cancel", L"rc", L"r/c", L"r c",
		 L"retry cancel"
		 L"r;c",
		 L"retry;cancel"},
		{L"Retry", L"Cancel"}
	),
	DefaultButtonOption(
		{L"yes/no", L"yn", L"y/n", L"y n", L"yes no", L"y;n", L"yes;no"},
		{L"Yes", L"No"}
	),
	DefaultButtonOption(
		{L"yes/no/cancel", L"ync", L"y/n/c", L"y n c", L"yes no cancel", L"y;n;c", L"yes;no;cancel"},
		{L"Yes", L"No", L"Cancel"}
	)
};

long getBoxType(wstring typeString) {
	// cSpell:disable
	if (typeString.starts_with(L"q"))
		return wxICON_QUESTION;
	if (typeString.starts_with(L"w"))
		return wxICON_WARNING;
	if (typeString.starts_with(L"e"))
		return wxICON_ERROR;
	return wxICON_INFORMATION;
	/* cSpell:enable */
}

map<wstring, wstring> parseArgs(int argc, wxCmdLineArgsArray &argv) {
	vector<wstring> keys = {L"title", L"content", L"type", L"buttons"};
	map<wstring, wstring> parsedArgs;
	if (argc < 4) {
		throw runtime_error("Less than 4 arguments passed!");
	}

	for (int i = 1; i < argc; i += 2) {
		string arg(argv[i]);
		wstring wArg = StringHelper::stringToWideString(arg);
		for (auto &key : keys) {
			if (wArg != L"--" + key)
				continue;
			string val(argv[i + 1]);
			wstring wVal = StringHelper::stringToWideString(val);
			if (wVal.starts_with(L"\"") && wVal.ends_with(L"\"")) {
				boost::algorithm::erase_first(wVal, L"\"");
				boost::algorithm::erase_last(wVal, L"\"");
			}
			parsedArgs[key] = wVal;
		}
	}
	return parsedArgs;
}

int getButtonCount(wstring buttonsString) {
	vector<boost::iterator_range<wstring::const_iterator>> results;
	boost::algorithm::find_all(results, buttonsString, L",");
	return results.size() + 1;
}

CustomBox *createBox(wstring title, wstring message, wstring typeString, wstring buttons) {
	CustomBox *box = new CustomBox(message, title, getBoxType(typeString), parseButtons(buttons));
	return box;
}

vector<wstring> parseButtons(wstring buttonsString) {
	for (auto &option : defaultOptions) {
		if (any_of(
					option.validInputs.begin(), option.validInputs.end(),
					[&buttonsString](wstring item) { return boost::iequals(item, buttonsString); }
				)) {
			return option.resultingButtons;
		}
	}
	vector<wstring> results = StringHelper::splitString(buttonsString, L";");
	for (auto &item : results) {
		boost::algorithm::trim(item);
		for (auto &btn : buttonsToCapitalize) {
			if (!boost::iequals(item, btn))
				continue;
			item = btn;
		}
	}
	return results;
}

void CustomBox::_handleButtons() {
	switch (_buttons.size()) {
		case 1:
			SetOKLabel(ButtonLabel(_buttons[0]));
			break;
		case 2:
			SetYesNoLabels(ButtonLabel(_buttons[0]), ButtonLabel(_buttons[1]));
			break;
		default:
			SetYesNoCancelLabels(
				ButtonLabel(_buttons[0]), ButtonLabel(_buttons[1]), ButtonLabel(_buttons[2])
			);
	}
}

bool MessageBoxApp::OnInit() {
	HICON iconHandle = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wxIcon icon;
	icon.CreateFromHICON(iconHandle);
	DestroyIcon(iconHandle);
	parsedArgs = parseArgs(wxApp::argc, wxApp::argv);
	CustomBox *box = createBox(
		parsedArgs[L"title"], parsedArgs[L"content"], parsedArgs[L"type"], parsedArgs[L"buttons"]
	);
	box->SetIcon(icon);
	int response = box->ShowModal();
	SetTopWindow(box);
	box->SetFocus();
	box->EndModal(0);
	delete box;
	exit(0);
	return true;
}

wxIMPLEMENT_APP(MessageBoxApp);
