#pragma once

#include "TestCase.h"
#include "HappyMath/PolygonMesh.h"

class TestMeshSetOps : public TestCase
{
public:
	TestMeshSetOps();
	virtual ~TestMeshSetOps();

	virtual bool Setup() override;
	virtual bool Render() override;

protected:
	HappyMath::PolygonMesh cutMeshA, cutMeshB;
};