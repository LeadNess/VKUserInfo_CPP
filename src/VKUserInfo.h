#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <codecvt>
#include <Windows.h>

#include "api.h"

using std::cout;
using std::wcout;

using std::cin;
using std::endl;

namespace VK {

	using namespace std::chrono;

	using std::string;
	using std::wstring;

	using std::to_string;
	using std::to_wstring;

	using std::ifstream;
	using std::ofstream;

	class VKUserInfo : public Client {

	private:

		int _user_id;
		string _user_name;
		string _user_surname;

	public:

		VKUserInfo() {};

		explicit VKUserInfo(const int& id);
		explicit VKUserInfo(const string& domain);

		~VKUserInfo() {};

		json userAllInfo();
		json downloadUserInfo();

		int getUserID() const;
		string getName() const;
		string getSurname() const;

		json userMainInfo();
		json userStatus();
		json userFriends();
		json userFollowers();
		json userPhotos();
		json userGroups();
		json userWall();

	};

	void Sleep(const int& millisec);

} // namespace VK