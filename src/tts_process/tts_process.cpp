#include "tts_process.hpp"

using namespace std;


void loadFileInResource(int name, DWORD& size, const char*& data) {
	HMODULE handle = ::GetModuleHandle(NULL);
	if (!handle) throw InteractBoxException(ErrorCodes::CannotFindResource);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(name), RT_RCDATA);
	HGLOBAL rcData = LoadResource(handle, rc);
	size = SizeofResource(handle, rc);
	data = static_cast<const char*>(::LockResource(rcData));
}

void replaceBannedWords(string& input, vector<string> bannedWords, string replacement) {
	// Using a magic word to prevent it from partially redacting the word "raccoon"
	string magicWord = "djsfoijw9fjef9832490rfsd0fjds89fusd98fs";
	boost::ireplace_all(input, "raccoon", magicWord);
	for (auto word : bannedWords) {
		boost::ireplace_all(input, word, replacement);
	}
	boost::ireplace_all(input, magicWord, "raccoon");
}

int main(int argc, char const* argv[]) {
	const char* bannedWordsCharPointer;
	DWORD size = 0;
	try {
		loadFileInResource(IDR_BANNED_WORDS, size, bannedWordsCharPointer);
	} catch (InteractBoxException& e) {
		wstring workingDir = FileHelper::getWorkingDirectory();
		wstring message = StringHelper::stringToWideString(e.what());
		wstring params = L"--title \"INTERACT BOX ERROR\"";
		params += L" --content \"" + message + L"\"" +
			L" --type e" +
			L" --buttons ok";
		HINSTANCE instance = ShellExecute(
			NULL,
			L"open",
			L"message_box_process.exe",
			params.c_str(),
			workingDir.c_str(),
			SW_SHOW
		);
		return 1;
	}
	vector<string> bannedWords;
	boost::split(bannedWords, bannedWordsCharPointer, boost::is_any_of("\n"));

	// Check for input string
	if (argc < 2) return 1;

	HRESULT hr = ::CoInitialize(nullptr);
	if (FAILED(hr)) {
		return 1;
	}

	std::string input = argv[1];

	replaceBannedWords(input, bannedWords, " [REDACTED] ");

	// Initialize TTS
	ISpVoice* pVoice = nullptr;
	hr = ::CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
	if (!SUCCEEDED(hr)) {
		::CoUninitialize();
		return 1;
	}

	// Get voice
	ISpObjectTokenCategory* pCategory = nullptr;
	hr = SpGetCategoryFromId(SPCAT_VOICES, &pCategory);
	if (SUCCEEDED(hr)) {
		IEnumSpObjectTokens* pEnum = nullptr;
		hr = pCategory->EnumTokens(nullptr, nullptr, &pEnum);
		if (SUCCEEDED(hr)) {
			ISpObjectToken* pToken = nullptr;
			while (pEnum->Next(1, &pToken, nullptr) == S_OK) {
				WCHAR* pDescription = nullptr;
				hr = SpGetDescription(pToken, &pDescription);
				if (SUCCEEDED(hr)) {
					// Get Microsoft Sam if exists
					if (wcsstr(pDescription, L"Microsoft Sam") != nullptr) {
						pVoice->SetVoice(pToken);
						CoTaskMemFree(pDescription);
						pToken->Release();
						break;
					}
					CoTaskMemFree(pDescription);
				}
				pToken->Release();
			}
			pEnum->Release();
		}
		pCategory->Release();
	}
	pVoice->Speak(StringHelper::stringToWideString(input).c_str(), SPF_DEFAULT, nullptr);
	pVoice->Release();
	::CoUninitialize();
	return 0;
}