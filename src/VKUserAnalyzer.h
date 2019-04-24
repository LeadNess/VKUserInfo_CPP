#pragma once

#include <map>
#include <sstream>

#include "VKUserInfo.h"

namespace VK {

	using std::map;
	using std::pair;

	class VKUserAnalyzer {

	private:

		VKUserInfo _user;
		json _jOldInf;
		json _jNewInf;

	public:

		explicit VKUserAnalyzer(const int& id);
		explicit VKUserAnalyzer(const std::string& domain);

		~VKUserAnalyzer() {};
		
		void getInfo(std::string& date);
		void getChanges();

	private:

		json cmpMainInfo();
		json cmpStatus();
		json cmpFriends();
		json cmpFollowers();
		json cmpGroups();
		json cmpPhotos();
		json cmpWall();

		wstring getDate();
	};

}  // namespace VK