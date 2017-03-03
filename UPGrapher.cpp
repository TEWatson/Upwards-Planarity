#include "stdafx.h"

#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/upward/LayerBasedUPRLayout.h>
#include <ogdf/upward/UpwardPlanarity.h>
#include <json/json.h>


using namespace ogdf;

class UPGrapher {

private:
	Graph graph;
	GraphAttributes attr; //may not be necessary to retain this as a member
	string inputFile = "";
	string outputFile = "";
	bool graphLoaded = false;

	Graph GraphFromJSON(string JSONdoc) {
		Json::Reader stringReader;
		Json::Value JSONObject;
		bool success = stringReader.parse(JSONdoc, JSONObject, true);
		if (!success) {
			// need error code here
		}
		return GraphFromJSON(JSONObject);
	}

	// currently we ignore the directedness value, we'll just assume it is directed for now
	Graph GraphFromJSON(Json::Value JSONobj) {
		Graph JSONGraph;
		Json::Value adjacencies = JSONobj["adjacencies"];
		std::vector<string> idList; // List of informal node identifiers
		std::vector<node> nodeList; // List of node objects
		for (Json::Value::iterator it = adjacencies.begin(); it != adjacencies.end(); ++it) { // create aligned lists for the nodes and identifiers
			string nodeString = ((*it).getMemberNames())[0]; // getting the first member, which will be the value name
			idList.push_back(nodeString);
			nodeList.push_back(JSONGraph.newNode());
		}
		
		Json::FastWriter fastWriter;
		for (Json::Value::iterator it = adjacencies.begin(); it != adjacencies.end(); ++it) { //create edges based on the adjacencies
			string sourceString = ((*it).getMemberNames())[0]; // next we iterate through the adjacencies
			std::vector<string>::iterator findSourceIndex = find(idList.begin(), idList.end(), sourceString);
			int index = (int)(findSourceIndex - idList.begin());
			node source = nodeList.at(index);

			//for (Json::Value::iterator arrayIter = (*it).begin(); arrayIter != (*it).end(); ++arrayIter) {
			//	node sink;
			//	string search = styledWriter.write((*arrayIter));
			//	int size = (*arrayIter).size();
			//	std::vector<string>::iterator findSinkIndex = find(idList.begin(), idList.end(), search);
			//	int index = (int)(findSinkIndex - idList.begin());
			//	sink = nodeList.at(index);
			//	JSONGraph.newEdge(source, sink); // and finally make an edge
			//}

			for (int arrayIter = 0; arrayIter < (*((*it).begin())).size(); arrayIter++) {
				node sink;
				string search = fastWriter.write((*(*it).begin())[arrayIter]);
				search.string::erase(std::remove(search.begin(), search.end(), '\"'), search.end());
				search.string::erase(std::remove(search.begin(), search.end(), '\n'), search.end());
				std::vector<string>::iterator findSinkIndex = find(idList.begin(), idList.end(), search);
				int index = (int)(findSinkIndex - idList.begin());
				sink = nodeList.at(index);
				JSONGraph.newEdge(source, sink); // and finally make an edge
			}
		}
		return JSONGraph;
	}

public:
	UPGrapher() {
		GraphAttributes GA(graph, GraphAttributes::nodeGraphics |
			GraphAttributes::edgeGraphics | GraphAttributes::edgeArrow);
		attr = GA;
	}

	UPGrapher(Graph g) {
		graph = g;
		GraphAttributes GA(graph, GraphAttributes::nodeGraphics |
			GraphAttributes::edgeGraphics | GraphAttributes::edgeArrow);
		attr = GA;
		graphLoaded = true;
	}

	UPGrapher(Graph g, string in, string out) {
		graph = g;
		inputFile = in;
		outputFile = out;
		GraphAttributes GA(graph, GraphAttributes::nodeGraphics |
			GraphAttributes::edgeGraphics | GraphAttributes::edgeArrow);
		attr = GA;
		graphLoaded = true;
	}

	void SetGraph(Graph g) {
		graph = g;
		GraphAttributes GA(graph, GraphAttributes::nodeGraphics |
			GraphAttributes::edgeGraphics | GraphAttributes::edgeArrow);
		attr = GA;
		graphLoaded = true;
	}

	void SetAttributes(GraphAttributes ga) {
		attr = ga;
	}

	void SetInputFile(string in) {
		inputFile = in;
	}

	string GetInputFile() {
		return inputFile;
	}

	void SetOutputFile(string out) {
		outputFile = out;
	}

	string GetOutputFile() {
		return outputFile;
	}

	void ReadAsGML() {
		GraphIO::read(graph, inputFile, GraphIO::readGML);
		graphLoaded = true;
	}

	void WriteAsGML() {
		GraphIO::write(graph, outputFile, GraphIO::writeGML);
	}

	void WriteAsGML(string explicitFile) {
		GraphIO::write(graph, explicitFile, GraphIO::writeGML);
	}

	void WriteAsSVG() {
		GraphIO::write(attr, outputFile, GraphIO::drawSVG);
	}

	void WriteAsSVG(string explicitFile) {
		GraphIO::write(attr, explicitFile, GraphIO::drawSVG);
	}

	void LoadGraphFromJSON(string JSONdoc) {
		graph = GraphFromJSON(JSONdoc);
		graphLoaded = true;
	}

	void LoadGraphFromJSON(Json::Value JSONobj) {
		graph = GraphFromJSON(JSONobj);
		graphLoaded = true;
	}

	bool DrawUPGraph() {
		DrawUPGraph(outputFile);
	}

	bool DrawUPGraph(string output) {
		if (graphLoaded == false) {
			return false;
		}
		else {
			adjEntry externalFaceAdj;
			bool isUpwardPlanar = UpwardPlanarity::embedUpwardPlanar(graph, externalFaceAdj); //attempt to embed as upward planar (if not possible, then ??)
			if (!isUpwardPlanar)
				return isUpwardPlanar;

			CombinatorialEmbedding emb(graph);
			emb.setExternalFace(emb.rightFace(externalFaceAdj)); //not well understood
			UpwardPlanRep upr(emb); //maybe add support for this object later on
			upr.augment();

			LayerBasedUPRLayout layout; //maybe add support for this object later on
			layout.call(upr, attr);

			// show edge arrows
			for (edge e : graph.edges) {
				attr.arrowType(e) = EdgeArrow::First;
			}

			outputFile = output;
			WriteAsSVG();

			return isUpwardPlanar;
		}
	}

};