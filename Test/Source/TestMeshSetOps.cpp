#include "TestMeshSetOps.h"
#include "HappyMath/Surface.h"
#include "HappyMath/LineSegment.h"
#include "HappyMath/Polygon.h"
#include <SDL3/SDL_opengl.h>

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

	// This is essential before we go into cutting one mesh against another, because the
	// algorthm that generates a mesh from a surface does not guarentee that all polygons
	// consist of coplanar vertices, and the algorithm that does the cutting assumes that
	// all polygons have coplanar vertices.
	meshA.TessellateFaces();
	meshB.TessellateFaces();

	std::vector<HappyMath::Polygon> polygonArrayA, polygonArrayB;
	
	if (!PolygonMesh::CalculateCutPolygons(meshA, meshB, polygonArrayA, polygonArrayB, this->cutSegmentArray))
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

	glLineWidth(2.0f);
	glBegin(GL_LINES);

	for (const LineSegment& cutSegment : this->cutSegmentArray)
	{
		glColor3d(1.0, 1.0, 1.0);
		glVertex3d(cutSegment.point[0].x, cutSegment.point[0].y, cutSegment.point[0].z);
		glVertex3d(cutSegment.point[1].x, cutSegment.point[1].y, cutSegment.point[1].z);
	}

	glEnd();

	return true;
}