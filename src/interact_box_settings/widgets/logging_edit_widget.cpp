#include "interact_box_settings/widgets/logging_edit_widget.hpp"



LoggingEditWidget::LoggingEditWidget(wxWindow* parent, int id, Json::Value& jsonSettings) : _jsonSettings(jsonSettings) {
	for (int i = LoggingLevel::DEBUG; i <= LoggingLevel::ERR; i++) {
		_choices.Add(loggingLevelToString(static_cast<LoggingLevel>(i)));
	}
	Create(parent, id, wxDefaultPosition, wxDefaultSize, _choices);
	SetSelection(_jsonSettings["loggingLevel"].asInt());
	Bind(wxEVT_CHOICE, &LoggingEditWidget::onChoice, this, id);
}

void LoggingEditWidget::onChoice(wxCommandEvent& event) {
	auto index = std::find(_choices.begin(), _choices.end(), event.GetString()) - _choices.begin();
	_jsonSettings["loggingLevel"] = index;
	event.Skip();
}
