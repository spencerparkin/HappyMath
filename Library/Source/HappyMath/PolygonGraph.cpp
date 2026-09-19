#include "HappyMath/PolygonGraph.h"
#include "HappyMath/Graph.h"
#include <set>
#include <map>

using namespace HappyMath;

//----------------------------- PolygonGraph -----------------------------

PolygonGraph::PolygonGraph()
{
}

/*virtual*/ PolygonGraph::~PolygonGraph()
{
	this->Clear();
}

void PolygonGraph::Clear()
{
	for (Node* node : this->nodeArray)
		delete node;

	this->nodeArray.clear();
}

bool PolygonGraph::Regenerate(const PolygonMesh& mesh, std::function<Node*()> nodeFactory /*= []() -> Node* { return new Node(); }*/)
{
	this->Clear();

	std::map<Graph::UnorderedEdge, Node*, Graph::UnorderedEdge> edgeMap;
	std::set<Graph::UnorderedEdge, Graph::UnorderedEdge> edgeSet;

	for (const PolygonMesh::Polygon& polygon : mesh.GetPolygonArray())
	{
		Node* node = nodeFactory();
		node->polygon = &polygon;
		node->adjacentNodeArray.resize(polygon.vertexArray.size());

		for (int i = 0; i < (int)polygon.vertexArray.size(); i++)
		{
			Graph::UnorderedEdge edge;
			edge.i = polygon.vertexArray[i];
			edge.j = polygon.vertexArray[polygon.Mod(i + 1)];

			if (edgeSet.find(edge) != edgeSet.end())
				return false;

			auto pair = edgeMap.find(edge);
			if (pair == edgeMap.end())
				edgeMap.insert(std::pair(edge, node));
			else
			{
				Node* adjacentNode = pair->second;
				const PolygonMesh::Polygon* adjacentPolygon = adjacentNode->polygon;

				if (node->adjacentNodeArray[i] != nullptr)
					return false;

				node->adjacentNodeArray[i] = adjacentNode;

				bool foundSlot = false;

				for (int j = 0; j < (int)adjacentPolygon->vertexArray.size(); j++)
				{
					if (adjacentPolygon->vertexArray[j] == edge.j)
					{
						if (adjacentNode->adjacentNodeArray[j] != nullptr)
							return false;

						adjacentNode->adjacentNodeArray[j] = node;
						foundSlot = true;
						break;
					}
				}

				if (!foundSlot)
					return false;

				edgeMap.erase(pair);
				edgeSet.insert(edge);
			}
		}

		this->nodeArray.push_back(node);
	}

	return true;
}

//----------------------------- PolygonGraph::Node -----------------------------

PolygonGraph::Node::Node()
{
	this->polygon = nullptr;
}

/*virtual*/ PolygonGraph::Node::~Node()
{
}