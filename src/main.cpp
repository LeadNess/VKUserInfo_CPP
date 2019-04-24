#include "interface.h"

int main() {
	setlocale(LC_ALL, "Russian");

	if (!programAuth()) {
		VK::Sleep(1000);
		return ERROR;
	}

	createDirectory();
	char choose;

	do {
		try {

			choose = menu();

			switch (choose) {

			case '1': downloadUserInfo();
				break;
			case '2': getAccountChanges();
				break;
			case '3': getManualActivity();
				break;
			case '4':
				break;
			default:
				cout << "Error: invalid menu option" << endl;
			}

		}
		catch (std::exception& e) {
			std::cerr << "Error: " << e.what() << endl;
			choose = '0';
		}
		catch (...) {
			choose = '0';
		}

	} while (choose != '4');

	return EXIT_SUCCESS;
}