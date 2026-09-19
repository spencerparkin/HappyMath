#pragma once

#include "TestCase.h"
#include "HappyMath/PolygonMesh.h"
#include "HappyMath/LineSegment.h"

class TestMeshSetOps : public TestCase
{
public:
	TestMeshSetOps();
	virtual ~TestMeshSetOps();

	virtual bool Setup() override;
	virtual bool Render() override;
	virtual void HandleController(XBoxController* controller) override;

protected:
	HappyMath::PolygonMesh cutMeshA, cutMeshB;
	bool renderMeshA;
	bool renderMeshB;
	std::vector<HappyMath::Vector3> intersectionArray;
};