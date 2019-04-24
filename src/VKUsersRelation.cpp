#include "VKUsersRelation.h"

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> to_utf8;

VK::string VK::VKUsersRelation::token;

VK::string VK::VKUsersRelation::getName(const string& domain) {
	string name;

	string url = "https://api.vk.com/method/users.get?user_ids=" + domain +	"&access_token=" + token + "&v=5.65";

	auto response = cpr::Get(cpr::Url{ url }, cpr::VerifySsl{ false });
	Sleep(350);

	json jres;

	try {
		jres = json::parse(response.text);
		name = jres.at("response")[0].at("first_name").get<string>();
		return name;
	}
	catch (...) {
		
	}

	return "";
}


VK::string VK::VKUsersRelation::getSurname(const string& domain) {
	string surname;

	string url = "https://api.vk.com/method/users.get?user_ids=" + domain + "&access_token=" + token + "&v=5.65";

	auto response = cpr::Get(cpr::Url{ url }, cpr::VerifySsl{ false });
	Sleep(350);

	json jres;

	try {
		jres = json::parse(response.text);
		surname = jres.at("response")[0].at("last_name").get<string>();
		return surname;
	}
	catch (...) {

	}

	return "";
}


int VK::VKUsersRelation::getUserID(const string& domain) {
	int id;

	string url = "https://api.vk.com/method/users.get?user_ids=" + domain + "&access_token=" + token + "&v=5.65";

	auto response = cpr::Get(cpr::Url{ url }, cpr::VerifySsl{ false });
	Sleep(350);

	json jres;

	try {
		jres = json::parse(response.text);
		id = jres.at("response")[0].at("id").get<int>();
		return id;
	}
	catch (...) {

	}

	return -1;
}


void VK::VKUsersRelation::setToken(const string& token) {
	this->token = token;
}


VK::VKUsersRelation::VKUsersRelation(const string& user1_domain, const wstring& date1, const string& user2_domain, const wstring& date2) {
	this->_user1_id = getUserID(user1_domain);
	
	wstring filename1 = to_utf8.from_bytes(getName(user1_domain)) + L'_' + to_utf8.from_bytes(getSurname(user1_domain));
	filename1 = filename1 + L"[id" + to_wstring(_user1_id) + L"].txt";

	ifstream user1_fin(wstring(L"../userdata/" + date1 + L'/' + filename1));

	if (!user1_fin.is_open()) throw std::exception("first user not found in userbase / invalid date");
	wcout << L"\nFirst user source file: " << filename1 << endl;


	this->_user2_id = getUserID(user2_domain);

	wstring filename2 = to_utf8.from_bytes(getName(user2_domain)) + L'_' + to_utf8.from_bytes(getSurname(user2_domain));
	filename2 = filename2 + L"[id" + to_wstring(_user2_id) + L"].txt";

	ifstream user2_fin(wstring(L"../userdata/" + date2 + L'/' + filename2));

	if (!user2_fin.is_open()) throw std::exception("second user not found in userbase / invalid date");
	wcout << L"Second user source file: " << filename2 << endl;


	user1_fin >> this->_jUser1;
	user1_fin.close();

	user2_fin >> this->_jUser2;
	user2_fin.close();
}


VK::pair<VK::json, VK::json> VK::VKUsersRelation::chechWall() {
	json jUser1Wall = _jUser1.at("wall");
	json jUser2Wall = _jUser2.at("wall");

	pair<json, json> jResult;
	int likes_count = 0, comm_count = 0;

	for (auto jPost : jUser1Wall["items"]) {
		int post_id = jPost.at("post_id").get<int>();
		json j;

		for (auto jLike : jPost["likes"]["items"])
			if (jLike.at("id").get<int>() == _user2_id) {

				j["post_id"] = post_id;
				j["activity"] = "like";
				likes_count++;

			}

		for (auto jComm : jPost["comments"]["items"])
			if (jComm.at("from_id").get<int>() == _user2_id) {

				j["post_id"] = post_id;
				j["comment"] = jComm;
				comm_count++;

			}

		if (j != json(nullptr))
			jResult.second["items"].emplace_back(j);
	}

	jResult.second["counters"]["likes"] = likes_count;
	jResult.second["counters"]["comments"] = comm_count;

	likes_count = 0;
	comm_count = 0;

	for (auto jPost : jUser2Wall["items"]) {
		int post_id = jPost.at("post_id").get<int>();
		json j;

		for (auto jLike : jPost["likes"]["items"])
			if (jLike.at("id").get<int>() == _user1_id) {

				j["post_id"] = post_id;
				j["activity"] = "like";
				likes_count++;

			}

		for (auto jComm : jPost["comments"]["items"])
			if (jComm.at("from_id").get<int>() == _user1_id) {

				j["post_id"] = post_id;
				j["comment"] = jComm;
				comm_count++;

			}

		if (j != json(nullptr))
			jResult.first["items"].emplace_back(j);
	}

	jResult.first["counters"]["likes"] = likes_count;
	jResult.first["counters"]["comments"] = comm_count;

	return std::move(jResult);
}


VK::pair<VK::json, VK::json> VK::VKUsersRelation::chechPhotos() {
	json jUser1Ph = _jUser1.at("photos");
	json jUser2Ph = _jUser2.at("photos");

	pair<json, json> jResult;
	int likes_count = 0, comm_count = 0;

	for (auto jPhoto : jUser1Ph["items"]) {
		int photo_id = jPhoto.at("photo_id").get<int>();
		json j;

		for (auto jLike : jPhoto["likes"]["items"])
			if (jLike.at("id").get<int>() == _user2_id) {

				j["photo_id"] = photo_id;
				j["activity"] = "like";
				likes_count++;

			}

		for (auto jComm : jPhoto["comments"]["items"])
			if (jComm.at("from_id").get<int>() == _user2_id) {

				j["photo_id"] = photo_id;
				j["comment"] = jComm;
				comm_count++;

			}

		if (j != json(nullptr))
			jResult.second["items"].emplace_back(j);
	}

	jResult.second["counters"]["likes"] = likes_count;
	jResult.second["counters"]["comments"] = comm_count;

	likes_count = 0;
	comm_count = 0;

	for (auto jPhoto : jUser2Ph["items"]) {
		int photo_id = jPhoto.at("photo_id").get<int>();
		json j;

		for (auto jLike : jPhoto["likes"]["items"])
			if (jLike.at("id").get<int>() == _user1_id) {

				j["photo_id"] = photo_id;
				j["activity"] = "like";
				likes_count++;

			}

		for (auto jComm : jPhoto["comments"]["items"])
			if (jComm.at("from_id").get<int>() == _user1_id) {

				j["photo_id"] = photo_id;
				j["comment"] = jComm;
				comm_count++;

			}

		if (j != json(nullptr))
			jResult.first["items"].emplace_back(j);
	}

	jResult.first["counters"]["likes"] = likes_count;
	jResult.first["counters"]["comments"] = comm_count;

	return std::move(jResult);
}


VK::json VK::VKUsersRelation::checkFriends() {
	json jUser1Fr = _jUser1.at("friends");
	json jUser2Fr = _jUser2.at("friends");

	json jResult;

	map<int, json> mapUser1Fr;

	for (auto jFriend : jUser1Fr["items"]) {

		int id = jFriend.at("id").get<int>();
		mapUser1Fr[id] = jFriend;

	}

	for (auto jFriend : jUser2Fr["items"]) {

		int id = jFriend.at("id").get<int>();

		if (mapUser1Fr.find(id) != mapUser1Fr.end())
			jResult["items"].emplace_back(jFriend);

	}

	jResult["count"] = jResult["items"].size();

	return std::move(jResult);
}


void VK::VKUsersRelation::getActivity() {
	pair<json, json> j;

	string us1_name = getName("id" + to_string(_user1_id));
	string us2_name = getName("id" + to_string(_user2_id));

	string us1_surname = getSurname("id" + to_string(_user1_id));
	string us2_surname = getSurname("id" + to_string(_user2_id));

	_jResult["user_1"]["info"] = json{ { "first_name", us1_name }, { "last_name", us1_surname }, { "id", _user1_id} };
	_jResult["user_2"]["info"] = json{ { "first_name", us2_name }, { "last_name", us2_surname }, { "id", _user2_id} };

	j = chechWall();

	_jResult["user_1"]["wall"] = j.first;
	_jResult["user_2"]["wall"] = j.second;


	j = chechPhotos();

	_jResult["user_1"]["photos"] = j.first;
	_jResult["user_2"]["photos"] = j.second;

	j.first = checkFriends();

	_jResult["manual_friends"] = j.first;

	wstring filename = to_utf8.from_bytes((getName("id" + to_string(_user2_id)))) + L'_' + to_utf8.from_bytes((getSurname("id" + to_string(_user2_id))));
	filename = filename + L"[1]_" + to_utf8.from_bytes((getName("id" + to_string(_user1_id)))) + L'_' + to_utf8.from_bytes((getSurname("id" + to_string(_user1_id)))) + L"[2].txt";

	ofstream fout(wstring(L"..\\userdata\\activity\\" + filename));

	fout << _jResult.dump(4) << endl;

	wcout << L"\nMutual activity of two users was loaded to " << filename << endl;

	fout.close();
}