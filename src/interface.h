#include "VKGraph.h"


char menu() {
	std::string choose;

	cout << "\n1. Download VK User information" << endl
		 << "2. Show VK Account changes"        << endl
		 << "3. Show VK Users activity"         << endl
		 << "4. Get VK social graph"            << endl
		 << "5. Exit program"                   << endl
		 << "\nChoose option: ";

	std::getline(cin, choose);

	return choose[0];
}


void downloadUserInfo() {
	std::string choose;

	cout << "1. Domain" << endl
		 << "2. ID" << endl
		 << "\nChoose option: ";

	std::getline(cin, choose);
	
	if (choose != "1" && choose != "2") {
		cout << "Error: invalid option" << endl;
		return;
	}

	if (choose[0] == '1') {
		std::string domain;

		cout << "Print domain: ";
		std::getline(cin, domain);

		VK::VKUserInfo api(domain);
		api.downloadUserInfo();
	}
	else {
		std::string id;

		cout << "Print ID: ";
		std::getline(cin, id);

		VK::VKUserInfo api(stoi(id));
		api.downloadUserInfo();
	}
}


void getAccountChanges() {
	std::string choose;

	cout << "1. Domain" << endl
		 << "2. ID" << endl
		 << "\nChoose option: ";

	std::getline(cin, choose);

	if (choose != "1" && choose != "2") {
		cout << "Error: invalid option" << endl;
		return;
	}

	if (choose[0] == '1') {
		std::string domain;

		cout << "Print domain: ";
		std::getline(cin, domain);

		std::string date;

		cout << "Date (example: 12.01.2018): ";
		std::getline(cin, date);

		VK::VKUserAnalyzer api(domain);
		api.getInfo(date);
		api.getChanges();
	}
	else {
		std::string id;

		cout << "Print ID: ";
		std::getline(cin, id);

		std::string date;

		cout << "Date (example: 12.01.2018): ";
		std::getline(cin, date);

		VK::VKUserAnalyzer api(stoi(id));
		api.getInfo(date);
		api.getChanges();
	}
}


void getManualActivity() {
	std::string domain1;

	cout << "\nPrint first user domain: ";
	std::getline(cin, domain1);

	std::wstring date1;

	cout << "Date (example: 12.01.2018): ";
	std::getline(std::wcin, date1);

	std::string domain2;

	cout << "\nPrint second user domain: ";
	std::getline(cin, domain2);

	std::wstring date2;

	cout << "Date (example: 12.01.2018): ";
	std::getline(std::wcin, date2);

	VK::VKUsersRelation api(domain1, date1, domain2, date2);
	api.getActivity();
}


void createDirectory() {
	SYSTEMTIME time;
	GetLocalTime(&time);

	std::stringstream stream;
	stream << std::setw(2) << std::right << std::setfill('0') << std::to_string(time.wDay)   << '.'
		   << std::setw(2) << std::right << std::setfill('0') << std::to_string(time.wMonth) << '.'
		   << std::setw(2) << std::right << std::setfill('0') << std::to_string(time.wYear);

	std::string foulder;
	stream >> foulder;

	std::string command = "md ..\\userdata\\" + foulder;

	system(command.c_str());
}




void userAuth() {
	std::string token;

	cout << "Enter token: ";
	std::getline(cin, token);

	std::fstream file("..\\token\\token.txt");
	file << token;
	file.close();

	VK::Client user;
	user.setToken(token);

	VK::VKUsersRelation vk_user;
	vk_user.setToken(token);

	if (!user.auth()) throw std::exception("authorization error");
}


void defaultAuth() {
	std::string token;

	std::ifstream fin("..\\token\\token.txt");
	fin >> token;
	fin.close();

	if (token.empty()) {
		cout << "\nNo default token\n"
		     << "Enter new token: ";
		std::getline(cin, token);
	}

	VK::Client user;
	user.setToken(token);

	VK::VKUsersRelation vk_user;
	vk_user.setToken(token);

	if (!user.auth()) throw std::exception("authorization error");
	
	std::ofstream fout("..\\token\\token.txt");
	fout << token;
	fout.close();
}


bool programAuth() {
	std::string option;

	cout << "1. User token\n"
		 << "2. Default token\n"
		 << "\nChoose option: ";

	std::getline(cin, option);

	try {
		switch (option[0]) {

		case '1': userAuth();
			break;
		case '2': defaultAuth();
			break;
		default:
			cout << "Error: invalid option" << endl;
			return false;

		}
		return true;
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << endl;
		return false;
	}
}


void getSocialGraph() {
	std::string domain, foulder;

	cout << "Print domain: ";
	std::getline(cin, domain);

	cout << "Print graph name: ";
	std::getline(cin, foulder);

	VK::VKGraph api(domain);
	api.getGraph(foulder, foulder);
}