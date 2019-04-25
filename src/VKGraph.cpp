#include "VKGraph.h"


VK::VKGraph::VKGraph(const string& domain) {
	this->_main_domain = domain;
}

void VK::VKGraph::getNodes() {

	VKUserInfo mainUser(_main_domain);
	json jMain = mainUser.userInfoForGraph();

	int mainID = mainUser.getUserID();
	string mainLabel = mainUser.getName() + ' ' + mainUser.getSurname();
	_mapNodes[pair<int, string>(mainID, mainLabel)] = jMain;

	for (auto& jFriend : jMain.at("friends").at("items")) {

		try {

			VKUserInfo user(jFriend.at("domain").get<string>());

			int id = user.getUserID();
			string label = user.getName() + ' ' + user.getSurname();

			_mapNodes[pair<int, string>(id, label)] = user.userInfoForGraph();

			Sleep(350);
		}
		catch (...) {
			cout << string("Error: bad data - user ") + jFriend.at("domain").get<string>() << endl;
		}

	}
}


void VK::VKGraph::getEdges() {
	for (auto& mainNode : _mapNodes) {
		for (auto& node : _mapNodes) {

			try {
				int id = node.second.at("main_info").at("id").get<int>();

				map<int, string> mapFriends;

				for (auto& Friend : mainNode.second.at("friends").at("items"))
					mapFriends[Friend.at("id").get<int>()] = Friend.at("domain").get<string>();

				if ((node != mainNode) && (mapFriends.find(id) != mapFriends.end())) {

					json jCompare = VKUsersRelation(mainNode.second, node.second).getGraphEdge();
					int weight = 1;

					weight += jCompare["mutual_groups"]["count"].get<int>();
					weight += jCompare["mutual_groups"]["count"].get<int>();

					int mainID = mainNode.second.at("main_info").at("id").get<int>();
					int ID = node.second.at("main_info").at("id").get<int>();

					_mapEdges[pair<int, int>(mainID, ID)] = weight;

				}
			}
			catch (...) {
				cout << "Error!" << endl;
			}
		}
	}
}


void VK::VKGraph::getGraph(const string& graphName, const string& foulder) {

	getNodes();
	getEdges();

	string command = "md ..\\userdata\\graph\\" + foulder;
	system(command.c_str());

	ofstream fNodes("..\\userdata\\graph\\" + graphName + "\\Nodes.csv");

	fNodes << "Id;Label" << endl;
	for (auto& node : _mapNodes)
		fNodes << node.first.first << ';' << node.first.second << endl;
	fNodes.close();

	ofstream fEdges("..\\userdata\\graph\\" + graphName + "\\Edges.csv");

	fEdges << "Target;Source;Weight" << endl;
	for (auto& edge : _mapEdges)
		fEdges << edge.first.first << ';' << edge.first.second << ";" << edge.second << endl;
	fEdges.close();

}