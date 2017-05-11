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

std::wstring StringToWindowsString(const std::string& s)
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
	string jsonTrue = //TODO: fix bug where this is not properly drawn/embedded
		"{"
			"\"directed\" : true,"
			"\"adjacencies\" : "
				"["
					"{\"A\" : []},"
					"{\"B\" : [\"A\"]},"
					"{\"C\" : [\"B\", \"D\"]},"
					"{\"D\" : [\"A\", \"B\"]},"
					"{\"E\" : [\"C\", \"F\", \"A\"]},"
					"{\"F\" : [\"B\", \"A\"]}"
				"]"
		"}";

	string jsonFalse = //this example is upward planar but NOT single source
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
	
	//// reading from string to JSON
	//Json::Reader stringReader;
	//Json::Value readDoc;
	//bool success = stringReader.parse(jsonTrue, readDoc, true);

	//// reading from JSON to Graph
	//UPGrapher grapher = UPGrapher();
	//grapher.LoadGraphFromJSON(readDoc);
	//grapher.WriteAsGML("test.gml");

	//std::wstring stemp = StringToWindowsString(styledWriter.write(readDoc));
	//OutputDebugString(stemp.c_str());

	//// embedding upward planar
	//bool embedded = grapher.DrawUPGraph("example4.svg");
	//if (embedded)
	//	stemp = StringToWindowsString("SUCCESS: successfully embedded the upward planar graph\n");
	//else
	//	stemp = StringToWindowsString("FAILURE: cannot find an upward planar embedding for this graph; make sure the graph is single source\n");
	//OutputDebugString(stemp.c_str());

	for (int i = 22; i < 23; i++) {
		for (int round = 0; round < 3; round++) {
			UPGrapher grapher = UPGrapher();
			string offFile = "";
			string gmlFile = "";
			string svgFile = "";
			if (round == 0) {
				offFile = "cont" + to_string(i) + ".off";
				gmlFile = "cont" + to_string(i) + ".gml";
				svgFile = "cont" + to_string(i) + ".svg";
			}
			else if (round == 1) {
				offFile = "aug" + to_string(i) + ".off";
				gmlFile = "aug" + to_string(i) + ".gml";
				svgFile = "aug" + to_string(i) + ".svg";
			}
			else {
				offFile = "m" + to_string(i) + ".off";
				gmlFile = "m" + to_string(i) + ".gml";
				svgFile = "m" + to_string(i) + ".svg";
			}
			grapher.LoadGraphFromOFF(offFile);
			grapher.WriteAsGML(gmlFile);
			string jstring = grapher.GetJSONAsString();
			//OutputDebugString(StringToWindowsString(jstring).c_str());

			std::wstring stemp;
			bool singleSource = hasSingleSource(grapher.GetGraphObject());
			bool embedded = false;
			try {
				embedded = grapher.DrawUPGraph(svgFile);
			}
			catch (std::bad_alloc& memoryError) {
				stemp = StringToWindowsString("Failure: encountered a memory exception -- continuing onto next graph\n\n");
				OutputDebugString(stemp.c_str());
				continue;
			}
			if (singleSource) {
				if (embedded)
					stemp = StringToWindowsString("SUCCESS: successfully embedded the upward planar graph\n");
				else
					stemp = StringToWindowsString("FAILURE: cannot find an upward planar embedding for this graph; make sure the graph is single source\n");
			}
			else {
				if (embedded) {
					stemp = StringToWindowsString("SUCCESS: successfully embedded the non-single-source upward planar graph\n");
				}
				else {
					stemp = StringToWindowsString("FAILURE: graph is not single source and could not be converted to upward planar\n");
					grapher.WriteAsGML("ssAttempt.gml");
				}
			}
			OutputDebugString(stemp.c_str());
		}
	}
	
	return 0;
}