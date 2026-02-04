#pragma once
#include <string>
#include <json/value.h>

namespace Structs {
	enum SettingType {
		JSON_BOOLEAN,
		INTEGER,
		STRING,
		ARRAY,
	};

	struct Setting {
			std::string name;
			Json::Value value;

			Setting() : name(""), value("") {}
			Setting(std::string n, Json::Value v) : name(n), value(v) {}
	};
} // namespace Structs
