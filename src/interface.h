#include "VKUsersRelation.h"
#include "sha1.h"


char menu() {
	std::string choose;

	cout << "\n1. Download VK User information" << endl
		 << "2. Show VK Account changes" << endl
		 << "3. Show VK Users activity" << endl
		 << "4. Exit program" << endl
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

	std::string command = "md ..\\userdata\\" + std::to_string(time.wDay) + '.' + std::to_string(time.wMonth) + '.' + std::to_string(time.wYear);

	system(command.c_str());
}




void userAuth() {
	std::string login;

	cout << "\nEnter login: ";
	std::getline(cin, login);

	std::string pass;

	cout << "Enter password: ";
	std::getline(cin, pass);

	SHA1 encrypt;

	encrypt.update(login);
	std::string login_hash = encrypt.final();

	encrypt.update(pass);
	std::string pass_hash = encrypt.final();

	std::string token;
	std::ifstream fin(std::string("..\\userdata\\auth\\" + login_hash + "\\" + pass_hash + ".txt"));

	if (!fin.is_open()) {
		std::string command = "md ..\\userdata\\auth\\" + login_hash;
		system(command.c_str());

		std::ofstream fout(std::string("..\\userdata\\auth\\" + login_hash + "\\" + pass_hash + ".txt"));

		cout << "Enter token: ";
		std::getline(cin, token);

		fout << token;
		fout.close();
	}

	fin >> token;
	fin.close();

	VK::Client user;

	user.setLogin(login);
	user.setPass(pass);
	user.setToken(token);

	VK::VKUsersRelation vk_user;
	vk_user.setToken(token);

	if (!user.auth()) {
		std::string path = "..\\userdata\\auth\\" + login_hash;
		system(std::string("DEL " + path + "\\" + pass_hash + ".txt").c_str());
		system(std::string("RD " + path).c_str());
		
		throw std::exception("authorization error");
	}
}


void defaultAuth() {
	std::string login = "89999651091";
	std::string pass = "Farantop1488";

	SHA1 encrypt;

	encrypt.update(login);
	std::string login_hash = encrypt.final();

	encrypt.update(pass);
	std::string pass_hash = encrypt.final();

	std::string token;

	std::ifstream fin(std::string("..\\userdata\\auth\\" + login_hash + "\\" + pass_hash + ".txt"));
	fin >> token;
	fin.close();

	VK::Client user;

	user.setLogin(login);
	user.setPass(pass);
	user.setToken(token);

	VK::VKUsersRelation vk_user;
	vk_user.setToken(token);
}


bool programAuth() {
	std::string option;

	cout << "1. Log in\n"
		 << "2. Use default token\n"
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