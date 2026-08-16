#include "HappyMath/Graph.h"
#include "HappyMath/PolygonMesh.h"
#include "HappyMath/Polygon.h"
#include "HappyMath/Function.h"
#include "HappyMath/Surface.h"
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

bool Graph::FromSurface(const Surface* surface, int minDegree, double walkDistance, const Vector3& probePoint)
{
	if (walkDistance <= 0.0 || !surface || minDegree < 2)
		return false;

	this->Clear();

	Node* node = new Node();
	this->nodeArray.push_back(node);

	if (!surface->FindNearestPoint(probePoint, node->vertex, node->normal))
		return false;

	std::set<Node*> nodeQueue;
	nodeQueue.insert(node);

	while (nodeQueue.size() > 0)
	{
		node = *nodeQueue.begin();
		nodeQueue.erase(node);

		while (node->adjacentNodeSet.size() < minDegree)
		{
			// Determine which direction we should try to travel along the surface from the node.
			Vector3 unitTangentDirection;
			switch (node->adjacentNodeSet.size())
			{
				case 0:
				{
					unitTangentDirection.SetAsOrthogonalTo(node->normal);
					break;
				}
				case 1:
				{
					Node* adjacentNode = *node->adjacentNodeSet.begin();
					unitTangentDirection = (node->vertex - adjacentNode->vertex).RejectedFrom(node->normal);
					break;
				}
				default:
				{
					Vector3 anchorDirection;
					anchorDirection.SetAsOrthogonalTo(node->normal);
					anchorDirection.Normalize();

					std::vector<Vector3> unitTangentDirectionArray;
					for (Node* adjacentNode : node->adjacentNodeSet)
						unitTangentDirectionArray.push_back((adjacentNode->vertex - node->vertex).RejectedFrom(node->normal).Normalized());

					std::sort(unitTangentDirectionArray.begin(), unitTangentDirectionArray.end(), [node, &anchorDirection](const Vector3& unitDirA, const Vector3& unitDirB) -> bool
						{
							double angleA = anchorDirection.AngleBetween(unitDirA, node->normal);
							double angleB = anchorDirection.AngleBetween(unitDirB, node->normal);
							return angleA < angleB;
						});

					double largestAngle = -1.0;
					for (int i = 0; i < (int)unitTangentDirectionArray.size(); i++)
					{
						int j = (i + 1) % unitTangentDirectionArray.size();
						const Vector3& unitTangentDirA = unitTangentDirectionArray[i];
						const Vector3& unitTangentDirB = unitTangentDirectionArray[j];
						double angle = unitTangentDirA.AngleBetween(unitTangentDirB, node->normal);
						if (angle > largestAngle)
						{
							largestAngle = angle;
							unitTangentDirection = unitTangentDirA.Rotated(node->normal, angle / 2.0);
						}
					}

					break;
				}
			}

			if (!unitTangentDirection.Normalize())
				return false;

			// Go in the direction of the tangent and probe the surface.
			Ray ray;
			ray.origin = node->vertex + unitTangentDirection * walkDistance;
			ray.unitDirection = -node->normal;
			std::unique_ptr<Node> tentativeNode(new Node());
			bool surfacePointFound = surface->RayCast(ray, tentativeNode->vertex, tentativeNode->normal);

			// If not found, then we're at a boundary point of the surface.
			// Be done, even if we haven't reached our minimum degree.
			if (!surfacePointFound)
				break;
			
			Node* newAdjacentNode = nullptr;

			double smallestSquareDistance = 0.0;
			Node* existingNode = this->FindClosestNode(tentativeNode->vertex, smallestSquareDistance);	// STPTODO: A box tree could possibly speed this up.
			if (existingNode && existingNode != node && (existingNode->vertex - tentativeNode->vertex).Length() < walkDistance)
			{
				newAdjacentNode = existingNode;
			}
			else
			{
				this->nodeArray.push_back(tentativeNode.get());
				nodeQueue.insert(tentativeNode.get());
				newAdjacentNode = tentativeNode.release();
			}

			// Again, here we may be, approximately, at a boundary-point of the surface.
			if (!newAdjacentNode)
				break;

			// We have to break out in this case or we'll loop indefinitely.
			if (node->IsAdjacentTo(newAdjacentNode))
				break;
			
			node->adjacentNodeSet.insert(newAdjacentNode);
			newAdjacentNode->adjacentNodeSet.insert(node);
		}
	}

	// STPTODO: Maybe now we delete any edges that cross other edges illegally?

	return true;
}

Graph::Node* Graph::FindClosestNode(const Vector3& vertex, double& smallestSquareDistance)
{
	Node* foundNode = nullptr;
	smallestSquareDistance = std::numeric_limits<double>::max();

	for (Node* node : this->nodeArray)
	{
		double squareDistance = (node->vertex - vertex).SquareLength();
		if (squareDistance < smallestSquareDistance)
		{
			smallestSquareDistance = squareDistance;
			foundNode = node;
		}
	}

	return foundNode;
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

void Graph::Dump(std::ostream& stream) const
{
	this->AssignIndicesForNodes();

	int size = (int)this->nodeArray.size();
	stream.write((char*)&size, sizeof(size));

	for (const Node* node : this->nodeArray)
		node->Dump(stream);
}

void Graph::Restore(std::istream& stream)
{
	this->Clear();

	int size = -1;
	stream.read((char*)&size, sizeof(size));

	for (int i = 0; i < size; i++)
	{
		Node* node = new Node();
		this->nodeArray.push_back(node);
		node->Restore(stream);
	}

	for (Node* node : this->nodeArray)
	{
		std::vector<int> offsetArray;
		for (Node* adjacentNode : node->adjacentNodeSet)
			offsetArray.push_back((int)(uintptr_t)adjacentNode);

		node->adjacentNodeSet.clear();

		for (int i : offsetArray)
			node->adjacentNodeSet.insert(this->nodeArray[i]);
	}
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

void Graph::Node::Dump(std::ostream& stream) const
{
	this->vertex.Dump(stream);
	this->normal.Dump(stream);

	int size = (int)this->adjacentNodeSet.size();
	stream.write((char*)&size, sizeof(size));

	for (const Node* node : this->adjacentNodeSet)
		stream.write((char*)&node->i, sizeof(node->i));
}

void Graph::Node::Restore(std::istream& stream)
{
	this->vertex.Restore(stream);
	this->normal.Restore(stream);

	int size = 0;
	stream.read((char*)&size, sizeof(size));

	this->adjacentNodeSet.clear();
	for (int i = 0; i < size; i++)
	{
		int j = -1;
		stream.read((char*)&j, sizeof(j));

		// These pointers will get patched by the caller.
		this->adjacentNodeSet.insert((Node*)(uintptr_t)j);
	}
}