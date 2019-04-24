#include "VKUserAnalyzer.h"

namespace VK {

	class VKUsersRelation {

	private:

		json _jUser1;
		json _jUser2;
		int _user1_id;
		int _user2_id;
		json _jResult;

		static string token;

	public:

		VKUsersRelation() {};
		explicit VKUsersRelation(const string& user1_domain, const wstring& date1, const string& user2_domain, const wstring& date2);
		~VKUsersRelation() {};

		void getActivity();

		pair<json, json> chechWall();
		pair<json, json> chechPhotos();
		json checkFriends();

		string getName(const string& domain);
		string getSurname(const string& domain);
		int getUserID(const string& domain);

		void setToken(const string& token);

	};

} // namespace VK
