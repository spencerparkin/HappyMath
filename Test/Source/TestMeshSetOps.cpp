#include "TestMeshSetOps.h"
#include "HappyMath/Surface.h"
#include "HappyMath/LineSegment.h"
#include "HappyMath/Polygon.h"

using namespace HappyMath;

TestMeshSetOps::TestMeshSetOps()
{
}

/*virtual*/ TestMeshSetOps::~TestMeshSetOps()
{
}

/*virtual*/ bool TestMeshSetOps::Setup()
{
	EllipticalDonutSurface ellipticalSurface;
	ellipticalSurface.A = 10.0;
	ellipticalSurface.B = 14.0;
	ellipticalSurface.girthRadius = 3.0;

	Graph graph;
	if (!graph.FromSurface(&ellipticalSurface, 5, 1.0, Vector3(1.0, 0.0, 0.0)))
		return false;

	PolygonMesh meshA;
	if (!graph.ToPolygonMesh(meshA))
		return false;

	SphereSurface sphericalSurface(Vector3(8.0, 0.0, 0.0), 10.0);

	if (!graph.FromSurface(&sphericalSurface, 5, 1.0, Vector3(1.0, 0.0, 0.0)))
		return false;

	PolygonMesh meshB;
	if (!graph.ToPolygonMesh(meshB))
		return false;

	std::vector<Polygon> polygonArrayA, polygonArrayB;
	std::vector<LineSegment> cutSegmentArray;

	if (!PolygonMesh::CalculateCutPolygons(meshA, meshB, polygonArrayA, polygonArrayB, cutSegmentArray))
		return false;

	this->cutMeshA.FromStandalonePolygonArray(polygonArrayA);
	this->cutMeshB.FromStandalonePolygonArray(polygonArrayB);

	this->cutMeshA.TessellateFaces();
	this->cutMeshB.TessellateFaces();

	return true;
}

/*virtual*/ bool TestMeshSetOps::Render()
{
	this->RenderMeshTriangles(this->cutMeshA);
	//this->RenderMeshTriangles(this->cutMeshB);

	return true;
}