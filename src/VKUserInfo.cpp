#include "VKUserInfo.h"

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;


void VK::Sleep(const int& millisec) {
	if (millisec < 0) throw std::invalid_argument("invalid argument");

	std::this_thread::sleep_for(std::chrono::milliseconds(millisec));
}


VK::VKUserInfo::VKUserInfo(const int& id) : _user_id(id) {
	if (!auth()) throw std::exception("no connection to the internet");

	json jres = call("users.get", "user_ids=" + to_string(id));

	if (jres.find("error") != jres.end()) throw std::exception("wrong id");

	try {
		this->_user_name = jres.at("response")[0].at("first_name").get<std::string>();
		this->_user_surname = jres.at("response")[0].at("last_name").get<std::string>();
	}
	catch (...) {

	}
};


VK::VKUserInfo::VKUserInfo(const std::string& domain) {
	if (!auth()) throw std::exception("no connection to the internet");

	json jres = call("users.get", "user_ids=" + domain);

	if (jres.find("error") != jres.end()) throw std::exception("wrong domain");

	try {
		this->_user_id = jres.at("response")[0]["id"].get<int>();

		this->_user_name = jres.at("response")[0].at("first_name").get<std::string>();
		this->_user_surname = jres.at("response")[0].at("last_name").get<std::string>();
	}
	catch (...) {

	}
};


VK::json VK::VKUserInfo::userMainInfo() {
	string fields = "&fields=counters,photo_id,verified,sex,bdate,city,country,home_town,domain,contacts,site,education,universities,schools,status,followers_count,occupation,relatives,relation,personal,connections,activities,interests,about,career";
	json jres;

	try {
		jres["main_info"] = call("users.get", "user_ids=" + to_string(_user_id) + fields).at("response")[0];

		jres.at("main_info").at("counters").erase("online_friends");
	}
	catch (...) {
		jres["main_info"] = json(nullptr);
	}

	Sleep(350);
	return std::move(jres);
}


VK::json VK::VKUserInfo::userFriends() {
	json jres;

	try {
		jres["friends"] = call("friends.get", "user_id=" + to_string(_user_id) + "&fields=domain,sex,bdate,country").at("response");

		for (auto& j : jres.at("friends")["items"])
			j.erase("online");
	}
	catch (...) {
		jres["friends"] = json(nullptr);
	}

	Sleep(350);
	return std::move(jres);
}


VK::json VK::VKUserInfo::userFollowers() {
	json jres;

	try {
		jres["followers"] = call("users.getFollowers", "user_id=" + to_string(_user_id) + "&fields=domain,sex,bdate,country").at("response");
	}
	catch (...) {
		jres["followers"] = json(nullptr);
	}

	Sleep(350);
	return std::move(jres);
}


VK::json VK::VKUserInfo::userPhotos() {
	json jPhotos, jres;

	try {
		jres = call("photos.getAll", "owner_id=" + to_string(_user_id) + "&count=200&photo_sizes=0&extended=0");

		jPhotos["photos"]["count"] = jres["response"]["items"].size();

		for (auto photo : jres["response"]["items"]) {
			int photo_id = photo["id"].get<int>();

			json j;

			try {
				j["likes"] = call("likes.getList", "&type=photo&owner_id=" + to_string(_user_id) + "&item_id=" + to_string(photo_id) + "&filter=likes&extended=1").at("response");
			}
			catch (...) {
				j["likes"] = json(nullptr);
			}

			Sleep(350);

			try {
				j["comments"] = call("photos.getComments", "&owner_id=" + to_string(_user_id) + "&photo_id=" + to_string(photo_id) + "&extended=1&fields=first_name,last_name").at("response");
			}
			catch (...) {
				j["comments"] = json(nullptr);
			}

			j["photo_id"] = photo_id;
			jPhotos["photos"]["items"].emplace_back(j);

			Sleep(350);
		}
	}
	catch (...) {
		jres["photos"] = json(nullptr);
	}


	return std::move(jPhotos);
}


VK::json VK::VKUserInfo::userGroups() {
	json jres;

	try {
		jres["groups"] = call("groups.get", "user_id=" + to_string(_user_id) + "&count=200&extended=1&fields=id,name,screen_name").at("response");

		for (auto& j : jres.at("groups")["items"]) {

			j.erase("is_advertiser");
			j.erase("is_member");
			j.erase("photo_100");
			j.erase("photo_200");
			j.erase("photo_50");

		}
	}
	catch (...) {
		jres["groups"] = json(nullptr);
	}

	Sleep(350);
	return std::move(jres);
}


VK::json VK::VKUserInfo::userStatus() {
	json jres;

	try {
		jres["status"] = call("status.get", "user_id=" + to_string(_user_id)).at("response");
	}
	catch (...) {
		jres["status"] = json(nullptr);
	}

	Sleep(350);
	return std::move(jres);
}


VK::json VK::VKUserInfo::userWall() {
	json jWall, jres;

	try {
		jres = call("wall.get", "owner_id=" + to_string(_user_id) + "&count=100&photo_sizes=0&extended=0");
		jWall["wall"]["count"] = jres["response"]["items"].size();

		for (auto post : jres["response"]["items"]) {

			int post_id = post["id"].get<int>();
			string text = post["text"].get<string>();

			json jAtt, j;

			try {
				j["likes"] = call("likes.getList", "&type=post&owner_id=" + to_string(_user_id) + "&item_id=" + to_string(post_id) +
					"&filter=likes&extended=1").at("response");
			}
			catch (...) {
				j["likes"] = json(nullptr);
			}

			try {
				Sleep(350);
				jAtt = post.at("attachments").get<json>();
			}
			catch (...) {

			}

			try {
				j["comments"] = call("wall.getComments", "&owner_id=" + to_string(_user_id) + "&post_id=" + to_string(post_id) +
					"&extended=1&fields=first_name,last_name").at("response");
			}
			catch (...) {
				j["comments"] = json(nullptr);
			}

			j["attachments"] = jAtt;
			j["text"] = text;
			j["post_id"] = post_id;

			jWall["wall"]["items"].emplace_back(j);

			Sleep(350);
		}
	}
	catch (...) {
		jres["wall"] = json(nullptr);
	}

	return std::move(jWall);
}


VK::json VK::VKUserInfo::userAllInfo() {
	json jInfo;

	auto start = steady_clock::now();
	long long time_last;

	wcout << L"\nStart loading information about "
		<< converter.from_bytes(_user_name) << L' ' << converter.from_bytes(_user_surname) << L"..."
		<< endl;


	jInfo["main_info"] = userMainInfo().at("main_info");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Main information was loaded... "
		<< endl;


	jInfo["status"] = userStatus().at("status");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Status was loaded... "
		<< std::endl;


	jInfo["friends"] = userFriends().at("friends");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Friends were loaded... "
		<< endl;


	jInfo["followers"] = userFollowers().at("followers");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Followers were loaded... "
		<< endl;


	jInfo["groups"] = userGroups().at("groups");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Groups were loaded... "
		<< endl;


	jInfo["wall"] = userWall().at("wall");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Wallposts were loaded... "
		<< endl;


	jInfo["photos"] = userPhotos().at("photos");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Photos were loaded... "
		<< endl;


	SYSTEMTIME time;
	GetLocalTime(&time);

	jInfo["date"]["year"] = time.wYear;
	jInfo["date"]["month"] = time.wMonth;
	jInfo["date"]["day"] = time.wDay;
	jInfo["date"]["hour"] = time.wHour;
	jInfo["date"]["minute"] = time.wMinute;

	wcout << L'[' << std::setw(3) << time_last << L" s]"
		<< L"Information about " + converter.from_bytes(_user_name) + L' ' + converter.from_bytes(_user_surname) + L" was successfully loaded."
		<< endl;

	return std::move(jInfo);
}


VK::json VK::VKUserInfo::downloadUserInfo() {
	json jInfo;

	auto start = steady_clock::now();
	long long time_last;

	wcout << L"\nStart loading information about "
		<< converter.from_bytes(_user_name) << L' ' << converter.from_bytes(_user_surname) << L"..."
		<< endl;


	jInfo["main_info"] = userMainInfo().at("main_info");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Main information was loaded... "
		<< endl;


	jInfo["status"] = userStatus().at("status");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Status was loaded... "
		<< endl;


	jInfo["friends"] = userFriends().at("friends");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Friends were loaded... "
		<< endl;


	jInfo["followers"] = userFollowers().at("followers");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Followers were loaded... "
		<< endl;


	jInfo["groups"] = userGroups().at("groups");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Groups were loaded... "
		<< endl;


	jInfo["wall"] = userWall().at("wall");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Wallposts were loaded... "
		<< endl;


	jInfo["photos"] = userPhotos().at("photos");

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();

	cout << '[' << std::setw(3) << time_last << " s]"
		<< "Photos were loaded... "
		<< endl;


	SYSTEMTIME time;
	GetLocalTime(&time);

	jInfo["date"]["year"] = time.wYear;
	jInfo["date"]["month"] = time.wMonth;
	jInfo["date"]["day"] = time.wDay;
	jInfo["date"]["hour"] = time.wHour;
	jInfo["date"]["minute"] = time.wMinute;

	wstring foulder = to_wstring(time.wDay) + L'.' + to_wstring(time.wMonth) + L'.' + to_wstring(time.wYear);

	wstring filename = converter.from_bytes(_user_name) + L'_' + converter.from_bytes(_user_surname) + L"[id" + to_wstring(_user_id) + L"].txt";

	ofstream fout(wstring(L"..\\userdata\\" + foulder + L'\\' + filename));
	fout << jInfo.dump(4) << std::endl;

	time_last = duration_cast<seconds>(steady_clock::now() - start).count();
	wcout << L'[' << std::setw(3) << time_last << L" s]"
		<< L"Information about " + converter.from_bytes(_user_name) + L' ' + converter.from_bytes(_user_surname)
		<< L" was successfully loaded to " + filename
		<< endl;

	fout.close();

	return std::move(jInfo);
}


int VK::VKUserInfo::getUserID() const {
	return this->_user_id;
}


VK::string VK::VKUserInfo::getName() const {
	return this->_user_name;
}


VK::string VK::VKUserInfo::getSurname() const {
	return this->_user_surname;
}