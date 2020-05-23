#pragma once

#include "api.h"


std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;


using ::std::string;
using ::std::wstring;
using json = ::nlohmann::json;


string VK::Client::login;
string VK::Client::pass;
string VK::Client::token;
string VK::Client::api_url = "https://api.vk.com/method/";
string VK::Client::auth_url = "https://oauth.vk.com/token?";
wstring VK::Client::wtoken;


VK::Client::Client(const std::string _version,
	const std::string _lang,
	const VK::callback_func_cap cap_callback,
	const VK::callback_func_fa2 _fa2_callback) :
	captcha_callback(cap_callback),
	fa2_callback(_fa2_callback),
	version(_version), lang(_lang)
{

}

std::string VK::Client::access_token() const {
	return a_t;
}

std::string VK::Client::last_error() const
{
	return l_error;
}


std::string VK::Client::get_captcha_key(const std::string &captcha_sid)
{
	return (captcha_callback != nullptr) ? captcha_callback(captcha_sid) : "";
}

std::string VK::Client::get_fa2_code()
{
	return (fa2_callback != nullptr) ? fa2_callback() : "";
}

bool VK::Client::check_access() {
	json jres = call("users.get", "");
	if (jres.find("error") != jres.end()) {
		this->clear();
		return false;
	}
	try {
		json info = jres.at("response").get<json>();
		info = info.begin().value();
	}
	catch (...) {
		this->clear();
		return false;
	}

	return true;
}

void VK::Client::setLogin(const string& login) {
	this->login = login;
}

void VK::Client::setPass(const string& pass) {
	this->pass = pass;
}

void VK::Client::setToken(const string& token) {
	this->token = token;
	this->wtoken = conv.from_bytes(token);
}

bool VK::Client::auth(const string &login, const string &pass, const string &access_token) {

	if (!access_token.empty()) {
		this->a_t = access_token;
		if (check_access()) {
			return true;
		}
	}

	if (login.empty() || pass.empty()) {
		return false;
	}

	return false;
}

json VK::Client::call(const string &method, const std::string &params) {
	if (method.empty()) {
		return nullptr;
	}
	string url = api_url + method;
	string data = params + ((params.empty()) ? "" : "&");

	params_map tmp_params;
	tmp_params.insert({ "captcha_sid", captcha_sid });
	tmp_params.insert({ "captcha_key", captcha_key });
	tmp_params.insert({ "v", version });
	tmp_params.insert({ "lang", lang });
	if (!a_t.empty()) {
		tmp_params.insert({ "access_token", a_t });
	}

	data += VK::Utils::data2str(tmp_params);
	string res = request(url, data);
	if (res.empty()) {
		return nullptr;
	}

	captcha_sid.clear();
	captcha_key.clear();

	try {
		json jres = json::parse(res);

		if (jres.find("error") == jres.end()) {
			return jres;
		}
		else {
			json item = jres.at("error").get<json>();
			string error = item.at("error_msg").get<string>();
			if (error == "need_captcha") {
				captcha_sid = item.at("captcha_sid").get<string>();
				captcha_key = get_captcha_key(captcha_sid);
				if (!captcha_key.empty()) {
					return this->call(method, params);
				}
				captcha_sid.clear();
				captcha_key.clear();
			}
			else {
				return jres;
			}

		}
	}
	catch (...) {

	}

	return nullptr;
}

json VK::Client::call(const wstring &method, const wstring &params) {
	if (method.empty()) {
		return nullptr;
	}

	wstring true_params;

	for (auto wc : params) {
		if (wc == L' ')
			true_params += L"%20";
		else
			true_params.push_back(wc);
	}

	wstring url = L"https://api.vk.com/method/" + method + L'?' + true_params + L"&access_token=" + wtoken + L"&v=5.65";

	auto wUrl = cpr::Url{ conv.to_bytes(url) };
	auto response = cpr::Get(wUrl, cpr::VerifySsl{ false });

	try {
		json jres = json::parse(response.text);
		return jres;
	}
	catch (...) {
		return json{ { "Error", response.error.message } };
	}
}


json VK::Client::sendMessage(const wstring &params) {
	if (params.empty()) {
		return nullptr;
	}

	return call(L"messages.send", params);
}



void VK::Client::clear()
{
	a_t.clear();

	captcha_sid.clear();
	captcha_key.clear();
	fa2_code.clear();
}


void VK::Client::set_fa2_callback(const VK::callback_func_fa2 _fa2_callback)
{
	fa2_callback = _fa2_callback;
}

void VK::Client::set_cap_callback(const VK::callback_func_cap cap_callback)
{
	captcha_callback = cap_callback;
}

json VK::Client::call(const string &method, const params_map &params) {

	if (method.empty()) {
		return nullptr;
	}

	string data;
	if (params.size()) {
		data = VK::Utils::data2str(params);
	}

	return this->call(method, data);
}




string VK::Utils::char2hex(const char dec) {
	char dig1 = (dec & 0xF0) >> 4;
	char dig2 = (dec & 0x0F);

	if (0 <= dig1 && dig1 <= 9) dig1 += 48;
	if (10 <= dig1 && dig1 <= 15) dig1 += 87;
	if (0 <= dig2 && dig2 <= 9) dig2 += 48;
	if (10 <= dig2 && dig2 <= 15) dig2 += 87;

	string r;
	r.append(&dig1, 1);
	r.append(&dig2, 1);
	return r;
}

string VK::Utils::urlencode(const string &url) {

	string escaped;
	for (const char& c : url) {
		if ((48 <= c && c <= 57) ||
			(65 <= c && c <= 90) ||
			(97 <= c && c <= 122) ||
			(c == '~' || c == '!' || c == '*' || c == '(' || c == ')' || c == '\'')
			)
			escaped.append(&c, 1);
		else {
			escaped.append("%");
			escaped.append(char2hex(c));
		}
	}

	return escaped;
}

string VK::Utils::data2str(const params_map &data) {
	string result;
	for (auto &kv : data) {
		result += kv.first + "=" + urlencode(kv.second) + "&";
	}

	return result;
}


int VK::Utils::CURL_WRITER(char *data, size_t size, size_t nmemb, string *buffer) {
	int result = 0;
	if (buffer != NULL) {
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}

	return result;
}


string VK::Client::request(const string &url, const string &data) {
	static char errorBuffer[CURL_ERROR_SIZE];

	curl_buffer.clear();

	CURL *curl;
	CURLcode result;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "VK API Client");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, VK::Utils::CURL_WRITER);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curl_buffer);
		result = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (result == CURLE_OK)
		{
			return curl_buffer;
		}
		else
		{
			return errorBuffer;
		}
	}
	curl_easy_cleanup(curl);

	return "";
}




