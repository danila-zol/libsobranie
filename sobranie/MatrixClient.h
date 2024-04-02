#include <string>
#include <rapidjson/document.h>

using std::string;

// MX ID parsing regex: @?(.*):(.*\..*)

class MatrixClient
{
public:
	string homeserver_url;
	string user_id;
	string auth_token;

	MatrixClient(string hs, string un);

	MatrixClient(string hs);

	char* hs_req(
		string req_type,
		string endpoint,
		struct curl_slist* headers,
		const char* data,
		size_t data_len
	);

	char* hs_post_req(
		string endpoint,
		struct curl_slist* headers,
		const char* data,
		size_t data_len
	)
	{
		return this->hs_req("POST", endpoint, headers, data, data_len);
	};

	char* hs_get_req(string endpoint, struct curl_slist* headers)
	{
		return this->hs_req("GET", endpoint, headers, NULL, 0);
	};

	char* hs_put_req(
		string endpoint,
		struct curl_slist* headers,
		const char* data,
		size_t data_len
	)
	{
		return this->hs_req("PUT", endpoint, headers, data, data_len);
	};

	string form_plogin_req(string password);

	string get_room_id(string room_name);

	int password_login(string password);

private:
	rapidjson::Document account_credentials;

	rapidjson::Document get_guest_acc();
};


	