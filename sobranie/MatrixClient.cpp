#include "MatrixClient.h"
#include "JsonFmt.h"
#include <curl/curl.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <iostream>
#include <format>
#include <fmt/core.h>
#include <string>

#define NULL_CHECK(expr) if(!(expr)) return NULL;

static size_t mem_cb(void* contents, size_t size, size_t nmemb, void* userp);


namespace MatrixEndpoints
{
	string guest_registration = "/_matrix/client/r0/register?kind=guest";
	string room_id_resolver = "/_matrix/client/v3/directory/room/";
	string password_login = "/_matrix/client/v3/login";
}


using std::string;

struct response {
	char* memory;
	size_t size;
};


static size_t mem_cb(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	struct response* mem = (struct response*) userp;

	char* ptr = (char*) realloc(mem->memory, mem->size + realsize + 1);
	if (!ptr)
	{
		/* out of memory! */
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = '\0';

	return realsize;
}


MatrixClient::MatrixClient(string hs, string un)
	: homeserver_url(hs), user_id(un)
{
	curl_global_init(CURL_GLOBAL_ALL);
}


MatrixClient::MatrixClient(string hs)
	: homeserver_url(hs)
{
	curl_global_init(CURL_GLOBAL_ALL);

	auto guest_creds = this->get_guest_acc();
	this->user_id = guest_creds["user_id"].GetString();
	this->auth_token = guest_creds["access_token"].GetString();
}


char* MatrixClient::hs_req(
	string req_type,
	string endpoint,
	struct curl_slist* headers,
	const char* data,
	size_t data_len
)
{
	CURL* handle = curl_easy_init();

	if (!headers)
	{
		headers = curl_slist_append(headers, "Content-Type: application/json");
	}

	if (!(this->auth_token.empty()))
	{
		string auth_header = "Authorization: Bearer " + this->auth_token;
		headers = curl_slist_append(headers, auth_header.c_str());
	}

	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, req_type.c_str());

	string url = MatrixClient::homeserver_url + endpoint;

	struct response resp = { (char*) malloc(0), 0 };

	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, mem_cb);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*) &resp);
	curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

	if (data)
	{
		curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
		curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, data_len);
	}

	curl_easy_perform(handle);

	curl_easy_cleanup(handle);

	curl_slist_free_all(headers);

	return resp.memory;
}

rapidjson::Document MatrixClient::get_guest_acc()
{
	const char* req = "{}";

	struct curl_slist* headers = NULL;
	curl_slist_append(headers, "Content-Type: application/json");

	rapidjson::Document g_acc_credentials;
	char* resp;

	resp = hs_post_req(MatrixEndpoints::guest_registration.c_str(), headers, req, strlen(req));
	g_acc_credentials.Parse(resp);

	free(resp);
	curl_slist_free_all(headers);

	return g_acc_credentials;
}


string MatrixClient::get_room_id(string room_name)
{
	string room_name_urlenc = curl_escape(room_name.c_str(), (int) room_name.size());
	string url = MatrixEndpoints::room_id_resolver + room_name_urlenc;

	char* resp = hs_get_req(url, NULL);
	
	rapidjson::Document r_json;
	r_json.Parse(resp);

	free(resp);

	rapidjson::Value::MemberIterator rv = r_json.FindMember("room_id");

	string rs = rv->value.GetString();

	return rs;
}


int MatrixClient::password_login(string password)
{
	using namespace rapidjson;

	Document rd;
	string req = this->form_plogin_req(password);

	struct curl_slist* headers = NULL;
	curl_slist_append(headers, "Content-Type: application/json");

	char* resp = hs_post_req(MatrixEndpoints::password_login, headers, req.c_str(), req.length());
	puts(resp);
	rd.Parse(resp);
	free(resp);
	curl_slist_free_all(headers);

	auto auth_key_val = rd.FindMember("access_token");
	this->auth_token = auth_key_val->value.GetString();

	return 1;
}


string MatrixClient::form_plogin_req(string password)
{
	using namespace rapidjson;

	StringBuffer sb;
	Writer<StringBuffer> writer(sb);

	std::string json_req = fmt::format(FMT::password_login, this->user_id, "I hate C++",  password);

	// Form login JSON request

	writer.StartObject();
	writer.Key("identifier");
	writer.StartObject();
	writer.Key("type"); writer.String("m.id.user");
	writer.Key("user"); writer.String(this->user_id.c_str());
	writer.EndObject();
	writer.Key("initial_device_display_name"); writer.String("I hate C++");
	writer.Key("password"); writer.String(password.c_str());
	writer.Key("type"); writer.String("m.login.password");
	writer.EndObject();

	return sb.GetString();
}
