#pragma once

#include "TestCase.h"

class TestSurfaceUnion : public TestCase
{
public:
	TestSurfaceUnion();
	virtual ~TestSurfaceUnion();

	virtual bool Setup() override;
	virtual bool Render() override;

protected:
	std::set<HappyMath::Graph::UnorderedEdge, HappyMath::Graph::UnorderedEdge> edgeSet;
	HappyMath::Graph graph;
	HappyMath::PolygonMesh mesh;
};