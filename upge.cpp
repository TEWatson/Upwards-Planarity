#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "stdafx.h"
#endif
#include <ogdf/upward/UpwardPlanarity.h>
#include "UPGrapher.cpp"
#include "json/json.h"

using namespace ogdf;

int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("usage for upge: upge [input .off file] [output gml file] [output svg file]\n");
		return 0;
	}
	
	string inputOFF = argv[1];
	string outputGML = argv[2];
	string outputSVG = argv[3];

	UPGrapher grapher = UPGrapher();
	grapher.LoadGraphFromOFF(inputOFF);
	grapher.WriteAsGML(outputGML);
	bool singleSource = hasSingleSource(grapher.GetGraphObject());
	bool embedded = false;
	try {
		embedded = grapher.DrawUPGraph(outputSVG);
	}
	catch (std::bad_alloc& memoryError) {
		printf("Encountered a memory error: this graph probably can't be handled in 32-bit OGDF implementation\n");
	}
	if (embedded) {
		printf("Successfully embedded the graph as upwards planar.\n");
	}
}