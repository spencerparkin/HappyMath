#include "TestPolygonCut.h"
#include <SDL3/SDL_opengl.h>
#include <fstream>

using namespace HappyMath;

TestPolygonCut::TestPolygonCut()
{
}

/*virtual*/ TestPolygonCut::~TestPolygonCut()
{
}

/*virtual*/ bool TestPolygonCut::Setup()
{
	std::ifstream fileStream;
	fileStream.open(R"(D:\tmp\data.bin)", std::ios::in | std::ios::binary);
	if (!fileStream.is_open())
		return false;

	this->polygonA.Restore(fileStream);
	this->polygonB.Restore(fileStream);
	fileStream.close();

	if (!this->lineSegment.Intersect(this->polygonA, this->polygonB))
		return false;

	bool test0 = this->polygonA.ContainsPoint(this->lineSegment.point[0]);
	bool test1 = this->polygonA.ContainsPoint(this->lineSegment.point[1]);
	bool test2 = this->polygonB.ContainsPoint(this->lineSegment.point[0]);
	bool test3 = this->polygonB.ContainsPoint(this->lineSegment.point[1]);

	return true;
}

/*virtual*/ bool TestPolygonCut::Render()
{
	this->RenderPolygonEdges(this->polygonA, Vector3(1.0, 0.0, 0.0));
	this->RenderPolygonEdges(this->polygonB, Vector3(0.0, 1.0, 0.0));

	this->RenderPolygonVertices(this->polygonA, Vector3(1.0, 0.0, 0.0));
	this->RenderPolygonVertices(this->polygonB, Vector3(0.0, 1.0, 0.0));

	glBegin(GL_LINES);

	glColor3d(0.0, 0.0, 1.0);

	glVertex3d(this->lineSegment.point[0].x, this->lineSegment.point[0].y, this->lineSegment.point[0].z);
	glVertex3d(this->lineSegment.point[1].x, this->lineSegment.point[1].y, this->lineSegment.point[1].z);

	glEnd();

	return true;
}