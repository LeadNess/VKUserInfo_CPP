#include "VKUsersRelation.h"

namespace VK {

	using std::multimap;

	class VKGraph {

	private:

		string _main_domain;

		map<pair<int, string>, json> _mapNodes;
		map<pair<int, int>, int> _mapEdges;

	public:

		VKGraph() {};
		explicit VKGraph(const string& domain);
		~VKGraph() {};

		void getGraph(const string& graphName, const string& foulder);

	private:

		void getNodes();
		void getEdges();
	};

} // namespace VK