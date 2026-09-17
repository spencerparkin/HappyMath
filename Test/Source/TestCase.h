#pragma once

#include "HappyMath/PolygonMesh.h"
#include "HappyMath/Graph.h"
#include <set>

class TestCase
{
public:
	TestCase();
	virtual ~TestCase();

	virtual bool Setup();
	virtual bool Shutdown();
	virtual bool Render() = 0;

protected:
	void RenderGraphNodes(const HappyMath::Graph* graph, const HappyMath::Vector3& color);
	void RenderEdgeSet(const std::set<HappyMath::Graph::UnorderedEdge, HappyMath::Graph::UnorderedEdge>& edgeSet, const HappyMath::Graph* graph, const HappyMath::Vector3& color);
	void RenderMeshTriangles(const HappyMath::PolygonMesh& polygonMesh);
	void RenderPolygonEdges(const HappyMath::Polygon& polygon, const HappyMath::Vector3& color);
	void RenderPolygonVertices(const HappyMath::Polygon& polygon, const HappyMath::Vector3& color);
};