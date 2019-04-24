#include "VKUserAnalyzer.h"

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;


VK::VKUserAnalyzer::VKUserAnalyzer(const int& id) {
	this->_user = VKUserInfo(id);
}

VK::VKUserAnalyzer::VKUserAnalyzer(const string& domain) {
	this->_user = VKUserInfo(domain);
}


void VK::VKUserAnalyzer::getInfo(string& date) {
	wstring filename = convert.from_bytes(_user.getName()) + L'_' + convert.from_bytes(_user.getSurname());

	filename = filename + L"[id" + to_wstring(_user.getUserID()) + L"].txt";

	ifstream fin(wstring(L"../userdata/" + convert.from_bytes(date) + L'/' + filename));

	if (!fin.is_open()) throw std::exception("user not found in userbase / invalid date");

	fin >> this->_jOldInf;
	fin.close();

	this->_jNewInf = _user.userAllInfo();
}


VK::json VK::VKUserAnalyzer::cmpMainInfo() {
	json jOldMain = this->_jOldInf.at("main_info");
	json jNewMain = this->_jNewInf.at("main_info");

	auto jOld = jOldMain.begin();
	auto jNew = jNewMain.begin();

	json jChange;

	for (; jOld != jOldMain.end() || jNew != jNewMain.end(); ++jOld, ++jNew)
		if (*jOld != *jNew) {

			if (jOld->is_primitive() && jNew->is_primitive()) {
				json j;

				j = { { jOld.key(), *jOld } };
				jChange["old"].emplace_back(j);

				j = { { jNew.key(), *jNew } };
				jChange["new"].emplace_back(j);
			}

			if (jOld->is_array() && jNew->is_array()) {
				json j;

				j = { { jOld.key(), *jOld } };
				jChange["old"].emplace_back(j);

				j = { { jNew.key(), *jNew } };
				jChange["new"].emplace_back(j);
			}

			if (jOld->is_object() && jNew->is_object()) {
				jChange["old"].emplace_back(*jOld);
				jChange["new"].emplace_back(*jNew);
			}
			
		}

	return std::move(jChange);
}


VK::json VK::VKUserAnalyzer::cmpStatus() {
	json oldStatus = this->_jOldInf.at("status");
	json newStatus = this->_jNewInf.at("status");

	json jChange;

	if (oldStatus == newStatus) {
		return json(nullptr);
	}

	jChange["old_status"] = oldStatus;
	jChange["new_status"] = newStatus;

	return std::move(jChange);
}


VK::json VK::VKUserAnalyzer::cmpFriends() {
	json jOldFriends = this->_jOldInf["friends"]["items"];
	json jNewFriends = this->_jNewInf["friends"]["items"];

	map<int, pair<json, bool>> mapCmp; // <friend_id, <friend_json, old/new>>

	for (auto j : jOldFriends) {
		int id = j["id"].get<int>();
		mapCmp[id] = make_pair(j, 0);
	}

	for (auto j : jNewFriends) {
		int id = j["id"].get<int>();

		if (mapCmp.find(id) == mapCmp.end())
			mapCmp[id] = make_pair(j, 1);
		else
			mapCmp.erase(id);
	}


	json jChange;

	for (auto& it : mapCmp) {

		if (it.second.second)
			it.second.first["status"] = "new friend";
		else
			it.second.first["status"] = "deleted from friends";

		jChange.emplace_back(it.second.first);
	}

	return std::move(jChange);
}


VK::json VK::VKUserAnalyzer::cmpFollowers() {
	json jOldFollowers = this->_jOldInf["followers"]["items"];
	json jNewFollowers = this->_jNewInf["followers"]["items"];

	map<int, pair<json, bool>> mapCmp; // <follower_id, <follower_json, old/new>>

	for (auto j : jOldFollowers) {
		int id = j["id"].get<int>();
		mapCmp[id] = make_pair(j, 0);
	}

	for (auto j : jNewFollowers) {
		int id = j["id"].get<int>();

		if (mapCmp.find(id) == mapCmp.end())
			mapCmp[id] = make_pair(j, 1);
		else
			mapCmp.erase(id);
	}


	json jChange;

	for (auto& it : mapCmp) {

		if (it.second.second)
			it.second.first["status"] = "new follower";
		else
			it.second.first["status"] = "deleted from followers";

		jChange.emplace_back(it.second.first);
	}

	return std::move(jChange);
}


VK::json VK::VKUserAnalyzer::cmpGroups() {
	json jOldGroups = this->_jOldInf["groups"]["items"];
	json jNewGroups = this->_jNewInf["groups"]["items"];

	map<int, pair<json, bool>> mapCmp; // <group_id, <group_json, old/new>>

	for (auto j : jOldGroups) {
		int id = j["id"].get<int>();
		mapCmp[id] = make_pair(j, 0);
	}

	for (auto j : jNewGroups) {
		int id = j["id"].get<int>();

		if (mapCmp.find(id) == mapCmp.end())
			mapCmp[id] = make_pair(j, 1);
		else
			mapCmp.erase(id);
	}


	json jChange;

	for (auto& it : mapCmp) {

		if (it.second.second)
			it.second.first["status"] = "new group";
		else
			it.second.first["status"] = "deleted from groups";

		jChange.emplace_back(it.second.first);
	}

	return std::move(jChange);
}


VK::json VK::VKUserAnalyzer::cmpPhotos() {
	json jOldPhotos = this->_jOldInf["photos"]["items"];
	json jNewPhotos = this->_jNewInf["photos"]["items"];

	map<int, pair<json, json>> mapOldPh; // <photo_id, <comments, likes> >
	map<int, pair<json, json>> mapNewPh;

	for (auto j : jOldPhotos) {
		int id = j["photo_id"].get<int>();

		mapOldPh[id] = make_pair(j.at("comments"), j.at("likes"));
	}

	for (auto j : jNewPhotos) {
		int id = j["photo_id"].get<int>();

		mapNewPh[id] = make_pair(j.at("comments"), j.at("likes"));
	}

	json jChange;

	std::vector<int> vecID;

	for(auto mOld : mapOldPh)
		if (mapNewPh.find(mOld.first) == mapNewPh.end()) {
			json jold;

			jold["post_id"] = mOld.first;
			jold["comments"] = mOld.second.first;
			jold["likes"] = mOld.second.second;
			jold["status"] = "deleted post";

			jChange.emplace_back(jold);
			vecID.emplace_back(mOld.first);
		}

	for (auto id : vecID)
		mapOldPh.erase(id);

	vecID.resize(0);
	
	for (auto mNew : mapNewPh)
		if (mapOldPh.find(mNew.first) == mapOldPh.end()) {
			json jnew;

			jnew["post_id"] = mNew.first;
			jnew["comments"] = mNew.second.first;
			jnew["likes"] = mNew.second.second;
			jnew["status"] = "new post";

			jChange.emplace_back(jnew);
			vecID.emplace_back(mNew.first);
		}
	
	for (auto id : vecID)
		mapNewPh.erase(id);

	auto mOld = mapOldPh.begin();
	auto mNew = mapNewPh.begin();

	json jChangeLikes, jChangeComm;

	for (int i = 0; i < mapOldPh.size(); ++i, ++mOld, ++mNew) {
		if ((mOld->second.first != mNew->second.first) || (mOld->second.second != mNew->second.second)) {

			json jOldComm = mOld->second.first;
			json jNewComm = mNew->second.first;

			if (jOldComm != jNewComm) {

				map<int, pair<json, bool>> mapCmp;

				for (auto j : jOldComm["items"]) {
					int id = j["id"].get<int>();
					mapCmp[id] = make_pair(j, 0);
				}

				for (auto j : jNewComm["items"]) {
					int id = j["id"].get<int>();

					if (mapCmp.find(id) == mapCmp.end())
						mapCmp[id] = make_pair(j, 1);
					else
						mapCmp.erase(id);
				}

				for (auto& it : mapCmp) {

					if (it.second.second)
						it.second.first["status"] = "new comment";
					else
						it.second.first["status"] = "deleted comment";

					jChangeComm.emplace_back(it.second.first);
				}
				
			}

			json jOldLikes = mOld->second.second;
			json jNewLikes = mNew->second.second;

			if (jOldLikes != jNewLikes) {

				map<int, pair<json, bool>> mapCmp;

				for (auto j : jOldLikes["items"]) {
					int id = j["id"].get<int>();
					mapCmp[id] = make_pair(j, 0);
				}

				for (auto j : jNewLikes["items"]) {
					int id = j["id"].get<int>();

					if (mapCmp.find(id) == mapCmp.end())
						mapCmp[id] = make_pair(j, 1);
					else
						mapCmp.erase(id);
				}

				for (auto& it : mapCmp) {

					if (it.second.second)
						it.second.first["status"] = "new like";
					else
						it.second.first["status"] = "deleted like";

					jChangeLikes.emplace_back(it.second.first);
				}

			}

			json j;

			if (jChangeComm != json(nullptr))
				j["comments"] = jChangeComm;

			if(jChangeLikes != json(nullptr))
				j["likes"] = jChangeLikes;

			if ((jChangeComm != json(nullptr)) || (jChangeLikes != json(nullptr))) {
				j["photo_id"] = mOld->first;
				jChange.emplace_back(j);
			}

			jChangeComm.clear();
			jChangeLikes.clear();
		}

	}

	return std::move(jChange);
}


VK::json VK::VKUserAnalyzer::cmpWall() {
	json jOldWall = this->_jOldInf["wall"]["items"];
	json jNewWall = this->_jNewInf["wall"]["items"];

	map<int, pair<json, json>> mapOldWall; // <post_id, <comments, likes> >
	map<int, pair<json, json>> mapNewWall;
	map<int, pair<string, string>> mapText; // <post_id, <old_text, new_text> >

	for (auto j : jOldWall) {
		int id = j["post_id"].get<int>();

		mapText[id].first = j["text"].get<string>();
		mapOldWall[id] = make_pair(j.at("comments"), j.at("likes"));
	}

	for (auto j : jNewWall) {
		int id = j["post_id"].get<int>();

		mapText[id].second = j["text"].get<string>();
		mapNewWall[id] = make_pair(j.at("comments"), j.at("likes"));
	}

	json jChange;

	std::vector<int> vecID;

	for (auto mOld : mapOldWall)
		if (mapNewWall.find(mOld.first) == mapNewWall.end()) {
			json jold;

			jold["post_id"] = mOld.first;
			jold["text"] = mapText[mOld.first];
			jold["comments"] = mOld.second.first;
			jold["likes"] = mOld.second.second;
			jold["status"] = "deleted post";

			jChange.emplace_back(jold);
			vecID.emplace_back(mOld.first);
		}

	for (auto id : vecID) {
		mapOldWall.erase(id);
		mapText.erase(id);
	}

	vecID.resize(0);

	for (auto mNew : mapNewWall)
		if (mapOldWall.find(mNew.first) == mapOldWall.end()) {
			json jnew;

			jnew["post_id"] = mNew.first;
			jnew["text"] = mapText[mNew.first];
			jnew["comments"] = mNew.second.first;
			jnew["likes"] = mNew.second.second;
			jnew["status"] = "new post";

			jChange.emplace_back(jnew);
			vecID.emplace_back(mNew.first);
		}

	for (auto id : vecID) {
		mapNewWall.erase(id);
		mapText.erase(id);
	}

	for (auto text : mapText)
		if (text.second.first != text.second.second) {

			json j = json{ { "old_text", text.second.first },{ "new_text", text.second.second },{ "post_id", text.first } };

			jChange.emplace_back(j);
		}

	auto mOld = mapOldWall.begin();
	auto mNew = mapNewWall.begin();

	json jChangeLikes, jChangeComm;
	
	for (int i = 0; i < mapOldWall.size(); ++i, ++mOld, ++mNew) {
		if ((mOld->second.first != mNew->second.first) || (mOld->second.second != mNew->second.second)) {

			json jOldComm = mOld->second.first;
			json jNewComm = mNew->second.first;

			if (jOldComm != jNewComm) {

				map<int, pair<json, bool>> mapCmp;

				for (auto j : jOldComm["items"]) {
					int id = j["id"].get<int>();
					mapCmp[id] = make_pair(j, 0);
				}

				for (auto j : jNewComm["items"]) {
					int id = j["id"].get<int>();

					if (mapCmp.find(id) == mapCmp.end())
						mapCmp[id] = make_pair(j, 1);
					else
						mapCmp.erase(id);
				}

				for (auto& it : mapCmp) {

					if (it.second.second)
						it.second.first["status"] = "new comment";
					else
						it.second.first["status"] = "deleted comment";

					jChangeComm.emplace_back(it.second.first);
				}

			}

			json jOldLikes = mOld->second.second;
			json jNewLikes = mNew->second.second;

			if (jOldLikes != jNewLikes) {

				map<int, pair<json, bool>> mapCmp;

				for (auto j : jOldLikes["items"]) {
					int id = j["id"].get<int>();
					mapCmp[id] = make_pair(j, 0);
				}

				for (auto j : jNewLikes["items"]) {
					int id = j["id"].get<int>();

					if (mapCmp.find(id) == mapCmp.end())
						mapCmp[id] = make_pair(j, 1);
					else
						mapCmp.erase(id);
				}

				for (auto& it : mapCmp) {
					if (it.second.second)
						it.second.first["status"] = "new like";
					else
						it.second.first["status"] = "deleted like";

					jChangeLikes.emplace_back(it.second.first);
				}

			}

			json j;

			if (jChangeComm != json(nullptr))
				j["comments"] = jChangeComm;

			if (jChangeLikes != json(nullptr))
				j["likes"] = jChangeLikes;

			if ((jChangeComm != json(nullptr)) || (jChangeLikes != json(nullptr))) {
				j["post_id"] = mOld->first;
				jChange.emplace_back(j);
			}

			jChangeComm.clear();
			jChangeLikes.clear();
		}

	}

	return std::move(jChange);
}


std::wstring VK::VKUserAnalyzer::getDate() {
	int hour   = this->_jOldInf.at("date").at("hour").get<int>();
	int minute = this->_jOldInf.at("date").at("minute").get<int>();
	int day    = this->_jOldInf.at("date").at("day").get<int>();
	int month  = this->_jOldInf.at("date").at("month").get<int>();
	int year   = this->_jOldInf.at("date").at("year").get<int>();

	std::wstringstream wstream;
	wstream << to_wstring(hour) << L'.'
		    << std::setw(2) << std::right << to_wstring(minute) << L'('
		    << std::setw(2) << std::right << to_wstring(day)    << L'.'
		    << std::setw(2) << std::right << to_wstring(month)  << L'.' 
		    << to_wstring(year) << L")-";

	hour   = this->_jNewInf.at("date").at("hour").get<int>();
	minute = this->_jNewInf.at("date").at("minute").get<int>();
	day    = this->_jNewInf.at("date").at("day").get<int>();
	month  = this->_jNewInf.at("date").at("month").get<int>();
	year   = this->_jNewInf.at("date").at("year").get<int>();

	wstream << to_wstring(hour) << L'.'
		    << std::setw(2) << std::right << to_wstring(minute) << L'(' 
		    << std::setw(2) << std::right << to_wstring(day)    << L'.' 
		    << std::setw(2) << std::right << to_wstring(month)  << L'.'
		    << to_wstring(year) << L')';

	wstring date;
	getline(wstream, date);
	
	return std::move(date);
}


void VK::VKUserAnalyzer::getChanges() {
	if (_jOldInf == json(nullptr) || _jNewInf == json(nullptr)) throw std::exception("user information was not loaded");

	json j, jChanges;

	j = cmpMainInfo();
	j == json(nullptr) ? jChanges["main_info"] = "no changes" : jChanges["main_info"] = j;

	j = cmpStatus();
	j == json(nullptr) ? jChanges["status"] = "no changes" : jChanges["status"] = j;

	j = cmpFriends();
	j == json(nullptr) ? jChanges["friends"] = "no changes" : jChanges["friends"] = j;

	j = cmpFollowers();
	j == json(nullptr) ? jChanges["followers"] = "no changes" : jChanges["followers"] = j;

	j = cmpGroups();
	j == json(nullptr) ? jChanges["groups"] = "no changes" : jChanges["groups"] = j;

	j = cmpPhotos();
	j == json(nullptr) ? jChanges["photos"] = "no changes" : jChanges["photos"] = j;

	j = cmpWall();
	j == json(nullptr) ? jChanges["wall"] = "no changes" : jChanges["wall"] = j;

	wstring filename = convert.from_bytes(_user.getName()) + L'_' + convert.from_bytes(_user.getSurname());
	filename = filename + L"[id" + to_wstring(_user.getUserID()) + L']' + L'[' + getDate() + L"].txt";

	ofstream fin(wstring(L"../userdata/comparison/" + filename));

	fin << jChanges.dump(4) << endl;
	wcout << L"Changes were successfully loaded to " + filename << endl;

	fin.close();
}