#pragma once

#include "HappyMath/PolygonMesh.h"
#include "HappyMath/Graph.h"
#include <set>

class XBoxController;

class TestCase
{
public:
	TestCase();
	virtual ~TestCase();

	virtual bool Setup();
	virtual bool Shutdown();
	virtual bool Render();
	virtual void HandleController(XBoxController* controller);

protected:
	void RenderGraphNodes(const HappyMath::Graph* graph, const HappyMath::Vector3& color);
	void RenderEdgeSet(const std::set<HappyMath::Graph::UnorderedEdge, HappyMath::Graph::UnorderedEdge>& edgeSet, const HappyMath::Graph* graph, const HappyMath::Vector3& color);
	void RenderMeshTriangles(const HappyMath::PolygonMesh& polygonMesh);
	void RenderMeshPolygons(const HappyMath::PolygonMesh& polygonMesh, int isolatedPolygon = -1);
	void RenderPolygonEdges(const HappyMath::Polygon& polygon, const HappyMath::Vector3& color);
	void RenderPolygonVertices(const HappyMath::Polygon& polygon, const HappyMath::Vector3& color);
	void RenderLineSegments(const std::vector<HappyMath::LineSegment>& lineSegmentArray);

	const HappyMath::Vector3& GetNextColor();

	std::vector<HappyMath::Vector3> colorArray;
	int colorOffset;
};