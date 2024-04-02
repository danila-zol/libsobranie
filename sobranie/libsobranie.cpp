#include <curl/curl.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <iostream>
#include <string>

#define NULL_CHECK(expr) if(!(expr)) return NULL;

// Evil global state
const std::string homeserver;
const std::string username;

struct response {
	char* memory;
	size_t size;
};

static size_t mem_cb(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	struct response* mem = (struct response*)userp;

	char* ptr = (char *) realloc(mem->memory, mem->size + realsize + 1);
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

//char* hs_post_req(
//	const char* endpoint,
//	struct curl_slist* headers,
//	const char* data,
//	size_t data_len
//)
//{
//	CURL* handle = curl_easy_init();
//
//	curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
//
//	// Form url
//	size_t url_len = strlen(endpoint) + hs_len + 2;
//	char* url;
//	NULL_CHECK(url = (char*) malloc(url_len));
//	snprintf(url, url_len, "%s%s", homeserver, endpoint);
//
//	struct response resp = { (char*) malloc(0), 0 };
//	rapidjson::Document r_json;
//
//	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, mem_cb);
//	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*) &resp);
//	curl_easy_setopt(handle, CURLOPT_URL, url);
//
//	if (data)
//	{
//		curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data);
//		curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, data_len);
//	}
//
//	curl_easy_perform(handle);
//
//	curl_easy_cleanup(handle);
//
//	return resp.memory;
//}
//
//inline char* hs_get_req(const char* endpoint, struct curl_slist* headers)
//{
//	return hs_post_req(endpoint, headers, NULL, 0);
//}
//
//int init_matrix_client(const char* hs, const char* un)
//{
//	hs_len = strlen(hs) + 1;
//	u_len = strlen(un) + 1;
//	char* tmp_s_buf;
//
//	// Prepare homeserver
//	NULL_CHECK(tmp_s_buf = (char*) malloc(hs_len));
//	memcpy((void*)tmp_s_buf, hs, hs_len);
//	homeserver = tmp_s_buf;
//
//	// Prepare user
//	NULL_CHECK(tmp_s_buf = (char*) malloc(u_len));
//	memcpy((void*)tmp_s_buf, un, u_len);
//	username = tmp_s_buf;
//
//	return 1;
//}
//
//rapidjson::Document get_guest_acc()
//{
//	const char* guest_reg_endpoint = "/_matrix/client/r0/register?kind=guest";
//	const char* req = "{}";
//
//	// Form headers
//	struct curl_slist* headers = NULL;
//	curl_slist_append(headers, "Content-Type: application/json");
//
//	rapidjson::Document g_acc_credentials;
//	char* resp;
//
//	resp = hs_post_req(guest_reg_endpoint, headers, req, strlen(req));
//	g_acc_credentials.Parse(resp);
//
//	free(resp);
//	curl_slist_free_all(headers);
//
//	return g_acc_credentials;
//}
//
//const char* get_room_id(const char* room_name)
//{
//	const char* resolver_endpoint = "/_matrix/client/v3/directory/room/";
//	const char* room_name_urlenc = curl_escape(room_name, 0);
//	size_t final_url_l = strlen(resolver_endpoint) + strlen(room_name_urlenc) + 1;
//	char* final_url = (char*) malloc(final_url_l);
//	snprintf(final_url, final_url_l, "%s%s", resolver_endpoint, room_name_urlenc);
//
//	char* resp = hs_get_req(final_url, NULL);
//
//	rapidjson::Document r_json;
//	r_json.Parse(resp);
//
//	free(resp);
//
//	rapidjson::Value::MemberIterator rv = r_json.FindMember("room_id");
//
//	if (rv == r_json.MemberEnd()) return NULL;
//	if (!rv->value.IsString()) return NULL;
//
//	size_t r_len = strlen(rv->value.GetString()) + 1;
//	char* rs;
//	NULL_CHECK(rs = (char*) malloc(r_len));
//	memcpy((void*) rs, (void*) rv->value.GetString(), r_len);
//
//	return rs;
//}
//
//void hand_of_abaddon()
//{
//
//	return;
//}
//
//void matrix_test()
//{
//	using namespace rapidjson;
//	init_matrix_client("https://matrix.org", "@foo:bar.com");
//
//	Document guest_acc_creds = get_guest_acc();
//	StringBuffer sb;
//	PrettyWriter<StringBuffer> writer(sb);
//	guest_acc_creds.Accept(writer);
//	puts(sb.GetString());
//	puts(get_room_id("#furrytech:matrix.org"));
//
//	return;
//}