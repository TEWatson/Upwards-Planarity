#include "stdafx.h"

#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/upward/LayerBasedUPRLayout.h>
#include <ogdf/upward/UpwardPlanarity.h>
#include <json/json.h>
#include <fstream>
#include <sstream>
#include <algorithm>

#define FIRST_LINE 0
#define NUMBERS 1
#define COORDS 2
#define FACES 3

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

			for (unsigned int arrayIter = 0; arrayIter < (*((*it).begin())).size(); arrayIter++) {
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

	void LoadGraphFromOFF(string filename) {
		ifstream offFile(filename);
		string line;
		int nvertices = 0;
		int nfaces = 0;
		std::vector<std::vector<int>> coordinates;
		std::vector<std::vector<int>> adjList;
		int faceOverrunCheck;
		//ignore the "OFF" header options, and assume dimension is 3
		int progressFlag = FIRST_LINE;
		while (getline(offFile, line)) {
			if (line[0] == '#')
				continue;
			switch (progressFlag) {
				case FIRST_LINE: {
					progressFlag = NUMBERS;
					continue;
				}
				case NUMBERS: {
					std::istringstream iss(line);
					iss >> nvertices >> nfaces;
					progressFlag = COORDS;
					continue;
				}
				case COORDS: { // load coordinates for later reference...
					std::istringstream iss(line);
					// assume there are three coordinates as mentioned above
					int x, y, z;
					iss >> x >> y >> z;
					std::vector<int> vertex = { x, y, z }; // x coordinate is not currently used
					std::vector<int> emptyAdjEntry = {};
					coordinates.push_back(vertex);
					adjList.push_back(emptyAdjEntry);
					if (coordinates.size() == nvertices)
						progressFlag = FACES;
					continue;
				}
				case FACES: { // use coordinates to determine direction of face edges; build adjacency list
					std::istringstream iss(line);
					int verticesInFace; // should always be three
					int vertices[3]; // since we're assuming dimension=3
					iss >> verticesInFace >> vertices[0] >> vertices[1] >> vertices[2];
					for (int i = 0; i < verticesInFace; i++) { // 3 not hardcoded for better looking code
						int vertexA = vertices[i];
						int vertexB = vertices[(i + 1) % 3];
						int Ay = coordinates[vertexA][1];
						int Az = coordinates[vertexA][2];
						int By = coordinates[vertexB][1];
						int Bz = coordinates[vertexB][2];
						int source = NULL;
						int sink = NULL;
						if (Ay < By) { // A -> B
							source = vertexA;
							sink = vertexB;
						}
						else if (Ay > By) { // B -> A
							source = vertexB;
							sink = vertexA;
						}
						else {
							if (Az < Bz) { // A -> B
								source = vertexA;
								sink = vertexB;
							}
							else if (Az > Bz) { // B -> A
								source = vertexB;
								sink = vertexA;
							}
							else {
								// A -> B for now...
								source = vertexA;
								sink = vertexB;
								// TODO: error checking or default decision needed here, maybe use the JSON directedness value?
								// This error is necessary because source and sink need a valid value for the below code to work properly
							}
						}
						std::vector<int> vectorCopy = adjList[source];
						if (find(vectorCopy.begin(), vectorCopy.end(), sink) != vectorCopy.end()) {
							// do nothing because this edge is redundant with a previous face
						}
						else {
							adjList[source].push_back(sink);
						}
					}
					faceOverrunCheck++;
					if (faceOverrunCheck >= nfaces) {
						break;
					}
					continue;
				}
				default: {}
			}
		}

		// Translate the created adjacency list to a JSON object
		Json::Value outputJsonObject;
		outputJsonObject["directed"] = true;
		//outputJsonObject["adjacencies"]
		Json::Value adjacencies;
		for (unsigned int i = 0; i < adjList.size(); i++) {
			std::vector<int> currentNode = adjList[i];
			Json::Value nodeName(Json::objectValue), nodeList(Json::arrayValue);
			for (unsigned int j = 0; j < currentNode.size(); j++) {
				nodeList.append(to_string(currentNode[j]));
			}
			nodeName[to_string(i)] = nodeList;
			adjacencies.append(nodeName);
		}

		outputJsonObject["adjacencies"] = adjacencies;
		LoadGraphFromJSON(outputJsonObject);
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
			if (hasSingleSource(graph)) {
				bool temp = UpwardPlanarity::isUpwardPlanar_triconnected(graph);
				bool isUpwardPlanar = UpwardPlanarity::embedUpwardPlanar(graph, externalFaceAdj); //attempt to embed as upward planar
				if (!isUpwardPlanar)
					return false;

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

				return true;
			}
			else {
				return false;
			}
		}
	}

};