#pragma once

#include "TestCase.h"
#include "HappyMath/Polygon.h"
#include "HappyMath/LineSegment.h"

class TestPolygonCut : public TestCase
{
public:
	TestPolygonCut();
	virtual ~TestPolygonCut();

	virtual bool Setup() override;
	virtual bool Render() override;

private:
	HappyMath::Polygon polygonA, polygonB;
	HappyMath::LineSegment lineSegment;
};