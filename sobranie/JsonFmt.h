#pragma once

namespace FMT {
	constexpr const char *password_login = R"({{
		"identifier": {{"type": "m.id.user", "user" : "{}"}},
		"initial_device_display_name" : {},
		"password" : {},
		"type" : "m.login.password" 
	}})";

	constexpr const char *change_displayname = R"({{
		"displayname": {}
	}})";

	constexpr const char *change_avatar = R"({{
		"avatar_url": {}
	}})";
}