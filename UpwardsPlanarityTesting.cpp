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