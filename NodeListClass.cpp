#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "stdafx.h"
#endif

#include <ogdf/basic/simple_graph_alg.h>
#include <algorithm>

using namespace ogdf;

class NodeListClass {

private:
	std::vector<node> nodeList;

public:
	NodeListClass() {
		//nothing to create
	}
	void clear() {
		nodeList.clear();
	}
	node at(int index) {
		return nodeList.at(index);
	}
	void pushBack(node nodeToPush) {
		nodeList.push_back(nodeToPush);
	}
	std::vector<node> getVector() {
		return nodeList;
	}
};