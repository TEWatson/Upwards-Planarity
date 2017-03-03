#include "stdafx.h"

#include <ogdf/basic/graph_generators.h>
#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/upward/LayerBasedUPRLayout.h>
#include <ogdf/upward/UpwardPlanarity.h>
#include "UPGrapher.cpp"
#include "json/json.h"
#include <Windows.h>
#include <vector>

using namespace ogdf;

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

int main()
{
	//Graph G;
	//randomHierarchy(G, 50, 100, true, true, false); //draws a random graph, not sure of the rules on this one
	//OGDF_ASSERT(isSimple(G));
	//makeSimpleUndirected(G);

	//UPGrapher upg = UPGrapher(G);
	//OGDF_ASSERT(upg.drawUPGraph("exampleGraph.svg"));

	string jsonFalse = //TODO: need to have a check for single source
		"{"
			"\"directed\" : true,"
			"\"adjacencies\" : "
				"["
					"{\"A\" : []},"
					"{\"B\" : [\"A\"]},"
					"{\"C\" : [\"B\", \"D\"]},"
					"{\"D\" : [\"A\"]},"
					"{\"E\" : [\"C\", \"F\"]},"
					"{\"F\" : [\"B\", \"D\"]}"
				"]"
		"}";

	string jsonTrue =
		"{"
			"\"directed\" : true,"
			"\"adjacencies\" : "
				"["
					"{\"A\" : [\"D\", \"E\"]},"
					"{\"B\" : [\"E\", \"F\"]},"
					"{\"C\" : [\"D\", \"E\"]},"
					"{\"D\" : []},"
					"{\"E\" : [\"F\", \"G\"]},"
					"{\"F\" : []},"
					"{\"G\" : []}"
				"]"
		"}";

	// Code to create the above JSON object manually (just as a reference)
	/*Json::Value graphObject(Json::objectValue);
	Json::Value adjList(Json::objectValue), adjacencies(Json::arrayValue);
	Json::Value A_node(Json::objectValue), A(Json::arrayValue);
	A.append("D");
	A.append("E");
	A_node["A"] = A;
	Json::Value B_node(Json::objectValue), B(Json::arrayValue);
	B.append("E");
	B.append("F");
	B_node["B"] = B;
	Json::Value C_node(Json::objectValue), C(Json::arrayValue);
	C.append("D");
	C.append("E");
	C_node["C"] = C;
	Json::Value D_node(Json::objectValue), D(Json::arrayValue);
	D_node["D"] = D;
	Json::Value E_node(Json::objectValue), E(Json::arrayValue);
	E.append("F");
	E.append("G");
	E_node["E"] = E;
	Json::Value F_node(Json::objectValue), F(Json::arrayValue);
	F_node["F"] = F;
	Json::Value G_node(Json::objectValue), G(Json::arrayValue);
	G_node["G"] = G;
	adjacencies.append(A_node);
	adjacencies.append(B_node);
	adjacencies.append(C_node);
	adjacencies.append(D_node);
	adjacencies.append(E_node);
	adjacencies.append(F_node);
	adjacencies.append(G_node);
	graphObject["adjacencies"] = adjacencies;
	graphObject["directed"] = true;*/

	Json::StyledWriter styledWriter; // human readable
	
	// reading from string to JSON
	Json::Reader stringReader;
	Json::Value readDoc;
	bool success = stringReader.parse(jsonTrue, readDoc, true);

	// reading from JSON to Graph
	UPGrapher grapher = UPGrapher();
	grapher.LoadGraphFromJSON(readDoc);
	grapher.WriteAsGML("test.gml");

	std::wstring stemp = s2ws(styledWriter.write(readDoc));
	OutputDebugString(stemp.c_str());

	// embedding upward planar
	// TODO: proper fail messages when not single-source
	//       stretch goal: embed non-single-source
	stemp = s2ws(to_string(grapher.DrawUPGraph("drawnJSON.svg")));
	OutputDebugString(stemp.c_str());

	return 0;
}