#include "TestMeshSetOps.h"
#include "HappyMath/Surface.h"
#include "HappyMath/LineSegment.h"
#include "HappyMath/Polygon.h"
#include "HappyMath/Transform.h"
#include "XBoxController.h"
#include <SDL3/SDL_opengl.h>

using namespace HappyMath;

TestMeshSetOps::TestMeshSetOps()
{
	this->renderMeshA = true;
	this->renderMeshB = true;
}

/*virtual*/ TestMeshSetOps::~TestMeshSetOps()
{
}

/*virtual*/ bool TestMeshSetOps::Setup()
{
#if 0
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
#endif

	PolygonMesh meshA, meshB;

	meshA.GeneratePolyhedron(PolygonMesh::Polyhedron::HEXADRON, 4.0);
	meshB.GeneratePolyhedron(PolygonMesh::Polyhedron::HEXADRON, 4.0);

	Transform transform;
	transform.translation.SetComponents(2.0, 2.0, 2.0);
	transform.TransformMesh(meshB);

	std::vector<HappyMath::Polygon> polygonArrayA, polygonArrayB;
	
	if (!PolygonMesh::CalculateCutPolygons(meshA, meshB, polygonArrayA, polygonArrayB, this->intersectionArray))
		return false;

	this->cutMeshA.FromStandalonePolygonArray(polygonArrayA);
	this->cutMeshB.FromStandalonePolygonArray(polygonArrayB);

	return true;
}

/*virtual*/ bool TestMeshSetOps::Render()
{
	if (this->renderMeshA)
		this->RenderMeshPolygons(this->cutMeshA);

	if (this->renderMeshB)
		this->RenderMeshPolygons(this->cutMeshB);

	glPointSize(4.0f);
	glBegin(GL_POINTS);

	for (const Vector3& point : this->intersectionArray)
	{
		glColor3d(1.0, 1.0, 1.0);
		glVertex3dv(&point.x);
	}

	glEnd();

	return true;
}

/*virtual*/ void TestMeshSetOps::HandleController(XBoxController* controller)
{
	if (controller->WasButtonPressed(XINPUT_GAMEPAD_X))
	{
		this->renderMeshA = !this->renderMeshA;
	}

	if (controller->WasButtonPressed(XINPUT_GAMEPAD_Y))
	{
		this->renderMeshB = !this->renderMeshB;
	}
}