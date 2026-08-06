#include "HappyMath/Graph.h"
#include "HappyMath/PolygonMesh.h"
#include "HappyMath/Polygon.h"
#include "HappyMath/Function.h"
#include <map>
#include <algorithm>
#include <assert.h>

using namespace HappyMath;

//--------------------------------- Graph ---------------------------------

Graph::Graph()
{
}

Graph::Graph(const Graph& graph)
{
	*this = graph;
}

/*virtual*/ Graph::~Graph()
{
	this->Clear();
}

void Graph::operator=(const Graph& graph)
{
	this->Clear();

	graph.AssignIndicesForNodes();

	for (const Node* otherNode : graph.nodeArray)
	{
		auto node = new Node();
		node->vertex = otherNode->vertex;
		node->normal = otherNode->normal;
		node->i = otherNode->i;
		this->nodeArray.push_back(node);
	}

	for (const Node* otherNode : graph.nodeArray)
	{
		Node* node = this->nodeArray[otherNode->i];

		for (const Node* otherAdjacentNode : otherNode->adjacentNodeSet)
		{
			Node* adjacentNode = this->nodeArray[otherAdjacentNode->i];
			node->adjacentNodeSet.insert(adjacentNode);
		}
	}
}

/*virtual*/ void Graph::Clear()
{
	for (Node* node : this->nodeArray)
		delete node;

	this->nodeArray.clear();
}

bool Graph::FromPolygohMesh(const PolygonMesh& mesh)
{
	this->Clear();

	for(int i = 0; i < mesh.GetNumVertices(); i++)
	{
		auto node = new Node();
		node->vertex = mesh.GetVertex(i);
		node->normal.SetComponents(0.0, 0.0, 0.0);
		this->nodeArray.push_back(node);
	}

	for (const PolygonMesh::Polygon& polygon : mesh.GetPolygonArray())
	{
		for (int i = 0; i < (signed)polygon.vertexArray.size(); i++)
		{
			int j = polygon.Mod(i + 1);

			Node* nodeA = this->nodeArray[polygon.vertexArray[i]];
			Node* nodeB = this->nodeArray[polygon.vertexArray[j]];

			if (!nodeA->IsAdjacentTo(nodeB))
				nodeA->adjacentNodeSet.insert(nodeB);

			if (!nodeB->IsAdjacentTo(nodeA))
				nodeB->adjacentNodeSet.insert(nodeA);
		}
	}

	for (const PolygonMesh::Polygon& polygon : mesh.GetPolygonArray())
	{
		Polygon standalonePolygon;
		polygon.ToStandalonePolygon(standalonePolygon, &mesh);
		Plane plane = standalonePolygon.CalcPlane();
		for (int i : polygon.vertexArray)
			this->nodeArray[i]->normal += plane.unitNormal;
	}

	for (Node* node : this->nodeArray)
		if (!node->normal.Normalize())
			return false;

	return true;
}

bool Graph::ToPolygonMesh(PolygonMesh& mesh, std::function<void(double)> progressCallback /*= {}*/) const
{
	Graph graph = *this;
	return graph.ToPolygonMesh(mesh, progressCallback);
}

bool Graph::ToPolygonMesh(PolygonMesh& mesh, std::function<void(double)> progressCallback /*= {}*/)
{
	mesh.Clear();

	int totalEdges = 0;
	int totalEdgesRemoved = 0;

	for (const Node* node : this->nodeArray)
	{
		mesh.AddVertex(node->vertex);
		totalEdges += (int)node->adjacentNodeSet.size();
	}

	this->AssignIndicesForNodes();

	PolygonMesh::Polygon polygon;
	int numEdgesRemoved = 0;
	while (this->FindAndRemovePolygonCycleForMesh(polygon.vertexArray, numEdgesRemoved))
	{
		mesh.AddPolygon(polygon);
		totalEdgesRemoved += numEdgesRemoved;
		if (progressCallback)
			progressCallback(double(totalEdgesRemoved) / double(totalEdges));
	}

	for (const Node* node : this->nodeArray)
		if (node->GetNumAdjacencies() > 0)
			return false;

	return true;
}

void Graph::AssignIndicesForNodes() const
{
	for (int i = 0; i < (signed)this->nodeArray.size(); i++)
		this->nodeArray[i]->i = i;
}

bool Graph::FindAndRemovePolygonCycleForMesh(std::vector<int>& cycleArray, int& numEdgesRemoved)
{
	numEdgesRemoved = 0;

	cycleArray.clear();

	Node* initialNode = nullptr;
	for (Node* node : this->nodeArray)
	{
		if (node->adjacentNodeSet.size() > 0)
		{
			initialNode = node;
			break;
		}
	}

	if (!initialNode)
		return false;

	Node* nodeIn = nullptr;
	Node* nodeOut = nullptr;
	Node* node = initialNode;

	do
	{
		cycleArray.push_back(node->i);

		if (node->adjacentNodeSet.size() == 0)
			return false;

		Node* chosenNode = *node->adjacentNodeSet.begin();
		if (nodeIn)
		{
			Vector3 vectorIn = (nodeIn->vertex - node->vertex).RejectedFrom(node->normal).Normalized();
			double smallestAngle = std::numeric_limits<double>::max();
			for (Node* adjacentNode : node->adjacentNodeSet)
			{
				if (adjacentNode == nodeIn)
					continue;

				nodeOut = adjacentNode;
				Vector3 vectorOut = (nodeOut->vertex - node->vertex).RejectedFrom(node->normal).Normalized();
				double angle = vectorOut.AngleBetween(vectorIn, node->normal);
				if (angle < smallestAngle)
				{
					smallestAngle = angle;
					chosenNode = nodeOut;
				}
			}
		}
		
		nodeOut = chosenNode;
		node->adjacentNodeSet.erase(nodeOut);
		nodeIn = node;
		node = nodeOut;
		nodeOut = nullptr;

		numEdgesRemoved++;
	} while (node->i != initialNode->i);

	return true;
}

bool Graph::ReduceEdgeCount(int numEdgesToRemove)
{
	// I'm not sure if this algorithm will be any good.
	// It's something to try.

	if (numEdgesToRemove <= 0)
		return true;

	std::set<UnorderedEdge, UnorderedEdge> edgeSet;
	this->GenerateEdgeSet<UnorderedEdge>(edgeSet);

	std::vector<UnorderedEdge> edgeArray;
	for (auto edge : edgeSet)
		edgeArray.push_back(edge);

	std::sort(edgeArray.begin(), edgeArray.end(), [this](const UnorderedEdge& edgeA, const UnorderedEdge& edgeB) -> bool
	{
		double lengthA = this->CalcEdgeLength(edgeA);
		double lengthB = this->CalcEdgeLength(edgeB);
		return lengthA < lengthB;
	});

	for (int i = 0; i < (signed)edgeArray.size(); i++)
	{
		if (numEdgesToRemove == 0)
			break;

		const Edge& edge = edgeArray[i];
		Node* nodeA = this->nodeArray[edge.i];
		Node* nodeB = this->nodeArray[edge.j];
		Node* newNode = this->MergeVertices(nodeA, nodeB);
		if (newNode)
			numEdgesToRemove--;
	}

	return numEdgesToRemove == 0;
}

double Graph::CalcEdgeLength(const Edge& edge) const
{
	return (this->nodeArray[edge.i]->vertex - this->nodeArray[edge.j]->vertex).Length();
}

Graph::Node* Graph::MergeVertices(Node* nodeA, Node* nodeB)
{
	if (!nodeA->IsAdjacentTo(nodeB) || !nodeB->IsAdjacentTo(nodeA))
		return nullptr;

	Vector3 unitDirection = nodeB->vertex - nodeA->vertex;
	if (!unitDirection.Normalize())
		return nullptr;

	Node* nodeU = nodeA->FindAdjacencyInDirection(-unitDirection);
	Node* nodeV = nodeB->FindAdjacencyInDirection(unitDirection);

	if (!nodeU || !nodeV)
		return nullptr;

	if (nodeU == nodeB || nodeV == nodeA)
		return nullptr;

	CubicSpaceCurve spaceCurve;
	if (!spaceCurve.FitToPoints(nodeU->vertex, nodeA->vertex, nodeB->vertex, nodeV->vertex, 0.0, 1.0 / 3.0, 2.0 / 3.0, 1.0))
		return nullptr;

	auto newNode = new Node();
	newNode->vertex = spaceCurve.Evaluate(0.5);
	newNode->normal = (nodeA->normal + nodeB->normal).Normalized();
	this->nodeArray.push_back(newNode);

	this->DeleteNode(nodeA, newNode);
	this->DeleteNode(nodeB, newNode);

	return newNode;
}

void Graph::DeleteNode(Node* node, Node* alternativeNode /*= nullptr*/)
{
	for (Node* existingNode : this->nodeArray)
	{
		if (existingNode->IsAdjacentTo(node))
		{
			existingNode->adjacentNodeSet.erase(node);
			if (alternativeNode)
				existingNode->adjacentNodeSet.insert(alternativeNode);
		}
	}

	for (int i = 0; i < (signed)this->nodeArray.size(); i++)
	{
		if (nodeArray[i] == node)
		{
			if (i != (signed)this->nodeArray.size() - 1)
				this->nodeArray[i] = this->nodeArray[this->nodeArray.size() - 1];
			this->nodeArray.pop_back();
			break;
		}
	}

	delete node;
}

void Graph::AddNode(Node* node)
{
	node->i = (int)this->nodeArray.size();
	this->nodeArray.push_back(node);
}

bool Graph::AddVerticesToBoxTree(BoxTree& boxTree)
{
	this->AssignIndicesForNodes();

	AxisAlignedBoundingBox boundingBox;
	boundingBox.MakeReadyForExpansion();

	for (Node* node : this->nodeArray)
		boundingBox.Expand(node->vertex);

	boundingBox.Scale(1.2);

	boxTree.Reset(boundingBox, boundingBox.GetVolume() / 32.0);

	for (Node* node : this->nodeArray)
	{
		auto nodeObject = std::make_shared<NodeObject>(this, node->i);
		if (!boxTree.InsertObject(nodeObject))
			return false;
	}

	return true;
}

bool Graph::AutoCompleteEdges(double localityRadius, int maxDegree, std::function<void(double)> progressCallback /*= {}*/)
{
	BoxTree boxTree;

	if (!this->AddVerticesToBoxTree(boxTree))
		return false;

	std::set<UnorderedEdge, UnorderedEdge> edgeSet;

	for (int i = 0; i < (int)this->nodeArray.size(); i++)
	{
		if (progressCallback)
			progressCallback(double(i) / double(this->nodeArray.size() - 1));

		Node* node = this->nodeArray[i];

		std::vector<std::shared_ptr<BoxTree::Object>> objectArray;
		if (!boxTree.FindObjectsOverlappingSphere(node->vertex, localityRadius, objectArray))
			continue;

		std::vector<Node*> localNodesArray;
		for (auto object : objectArray)
		{
			NodeObject* nodeObject = (NodeObject*)object.get();
			Node* localNode = const_cast<Node*>(nodeObject->GetNode());
			if (localNode != node)
				localNodesArray.push_back(localNode);
		}

		std::sort(localNodesArray.begin(), localNodesArray.end(), [node](const Node* nodeA, const Node* nodeB) -> int
			{
				double squareDistanceA = (node->vertex - nodeA->vertex).SquareLength();
				double squareDistanceB = (node->vertex - nodeB->vertex).SquareLength();

				return squareDistanceA < squareDistanceB;
			});

		for (int i = 0; i < maxDegree && i < (int)localNodesArray.size(); i++)
		{
			Node* localNode = localNodesArray[i];
			UnorderedEdge edge(node->i, localNode->i);
			if (edgeSet.find(edge) != edgeSet.end())
				continue;

			if (node->adjacentNodeSet.size() >= 2)
			{
				double largestAngle = -std::numeric_limits<double>::max();
				double smallestAngle = std::numeric_limits<double>::max();

				Node* immediateNodeCCW = nullptr;
				Node* immediateNodeCW = nullptr;

				Vector3 vectorA = (localNode->vertex - node->vertex).RejectedFrom(node->normal).Normalized();

				for (Node* adjacentNode : node->adjacentNodeSet)
				{
					Vector3 vectorB = (adjacentNode->vertex - node->vertex).RejectedFrom(node->normal).Normalized();

					double angle = vectorA.AngleBetween(vectorB, node->normal);

					if (angle > largestAngle)
					{
						largestAngle = angle;
						immediateNodeCW = adjacentNode;
					}

					if (angle < smallestAngle)
					{
						smallestAngle = angle;
						immediateNodeCCW = adjacentNode;
					}
				}

				assert(immediateNodeCCW && immediateNodeCW);
				
				if (immediateNodeCCW != immediateNodeCW)
				{
					double angle = smallestAngle + (2.0 * M_PI - largestAngle);

					if (angle < M_PI && immediateNodeCCW->IsAdjacentTo(immediateNodeCW))
					{
						// Don't make the connection, because this breaks and/or crosses an existing triangle in the mesh.
						continue;
					}
				}
			}

			node->adjacentNodeSet.insert(localNode);
			localNode->adjacentNodeSet.insert(node);

			edgeSet.insert(edge);
		}
	}

	return true;
}

//--------------------------------- Graph::NodeObject ---------------------------------

Graph::NodeObject::NodeObject(Graph* graph, int i)
{
	this->graph = graph;
	this->i = i;
}

/*virtual*/ Graph::NodeObject::~NodeObject()
{
}

/*virtual*/ AxisAlignedBoundingBox Graph::NodeObject::GetMinimalBoundingBox() const
{
	return AxisAlignedBoundingBox(this->GetNode()->GetVertex());
}

/*virtual*/ bool Graph::NodeObject::OverlapsSphere(const Vector3& center, double radius) const
{
	return (this->GetNode()->GetVertex() - center).SquareLength() <= radius * radius;
}

/*virtual*/ double Graph::NodeObject::CalcSquareDistanceToPoint(const Vector3& point) const
{
	return (this->GetNode()->GetVertex() - point).SquareLength();
}

const Graph::Node* Graph::NodeObject::GetNode() const
{
	return this->graph->GetNode(this->i);
}

//--------------------------------- Graph::Node ---------------------------------

Graph::Node::Node()
{
	this->i = -1;
}

/*virtual*/ Graph::Node::~Node()
{
}

bool Graph::Node::IsAdjacentTo(const Node* node) const
{
	return this->adjacentNodeSet.find(const_cast<Node*>(node)) != this->adjacentNodeSet.end();
}

Graph::Node* Graph::Node::FindAdjacencyInDirection(const Vector3& unitDirection)
{
	Node* foundNode = nullptr;
	double minAngle = std::numeric_limits<double>::max();
	for (Node* adjacentNode : this->adjacentNodeSet)
	{
		Vector3 unitAdjacentNodeDirection = (adjacentNode->vertex - this->vertex).Normalized();
		double angle = unitDirection.AngleBetween(unitAdjacentNodeDirection);
		if (angle < minAngle)
		{
			minAngle = angle;
			foundNode = adjacentNode;
		}
	}

	return foundNode;
}