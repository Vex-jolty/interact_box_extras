#include "interact_box_settings/widgets/array_edit_widget.hpp"
#include <sdkddkver.h>
using namespace std;

void ArrayEditWidget::onTextChange(wxCommandEvent& event) {
	vector<string> stringVec = StringHelper::splitString((string)event.GetString(), ", ");
	Json::Value newArray(Json::arrayValue);
	for (auto& item : stringVec) {
		newArray.append(item);
	}
	_arrayValue = newArray;
	_jsonSettings[_name] = _arrayValue;
	event.Skip();
}

void ArrayEditWidget::_setTextValue() {
	int size = _arrayValue.size();
	for (int i = 0; i < size; i++) {
		string text = (_arrayValue[i]).asString();
		if (i < size - 1) text += ", ";
		AppendText(text);
	}
}