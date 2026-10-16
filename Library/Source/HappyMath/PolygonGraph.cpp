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

PolygonGraph::Node* PolygonGraph::FindNodeForPolygon(const PolygonMesh::Polygon* polygon)
{
	for (PolygonGraph::Node* node : this->nodeArray)
		if (node->polygon == polygon)
			return node;

	return nullptr;
}

bool PolygonGraph::GenerateTriangleStrip(std::vector<int>& triangleStrip) const
{
	// TODO: This is untested and surely doesn't work.  Test it.

	triangleStrip.clear();

	for (const Node* triangle : this->nodeArray)
		if (triangle->polygon->vertexArray.size() != 3)
			return false;

	for (const Node* triangle : this->nodeArray)
		triangle->visited = false;

	while (true)
	{
		int i;
		for (i = 0; i < (int)this->nodeArray.size(); i++)
			if (!this->nodeArray[i]->visited)
				break;

		if (i == (int)this->nodeArray.size())
			break;

		const Node* triangle = this->nodeArray[i];

		if (triangleStrip.size() > 0)
			triangleStrip.push_back(-1);

		for (i = 0; i < 3; i++)
			triangleStrip.push_back(triangle->polygon->vertexArray[i]);

		int j = 0;
		i = 1;

		while (true)
		{
			triangle->visited = true;

			const Node* nextTriangle = triangle->adjacentNodeArray[i];
			if (!nextTriangle || nextTriangle->visited)
				break;

			for (i = 0; i < 3; i++)
				if (!triangle->polygon->HasVertex(nextTriangle->polygon->vertexArray[i]))
					break;

			HM_ASSERT(i != 3);

			triangleStrip.push_back(nextTriangle->polygon->vertexArray[i]);
			triangle = nextTriangle;

			if (++j % 2 == 0 && --i < 0)
				i = 2;
		}
	}

	return true;
}

//----------------------------- PolygonGraph::Node -----------------------------

PolygonGraph::Node::Node()
{
	this->polygon = nullptr;
	this->visited = false;
}

/*virtual*/ PolygonGraph::Node::~Node()
{
}