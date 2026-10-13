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
	this->renderDiffMesh = true;
}

/*virtual*/ TestMeshSetOps::~TestMeshSetOps()
{
}

/*virtual*/ bool TestMeshSetOps::Setup()
{
#if 1
	EllipticalDonutSurface ellipticalSurface;
	ellipticalSurface.A = 10.0;
	ellipticalSurface.B = 14.0;
	ellipticalSurface.girthRadius = 3.0;

	Graph graph;
	if (!graph.FromSurface(&ellipticalSurface, 5, 1.0, Vector3(1.0, 0.0, 0.0)))
		return false;

	if (!graph.ToPolygonMesh(this->meshA))
		return false;

	SphereSurface sphericalSurface(Vector3(8.0, 0.0, 0.0), 10.0);

	if (!graph.FromSurface(&sphericalSurface, 5, 1.0, Vector3(1.0, 0.0, 0.0)))
		return false;

	if (!graph.ToPolygonMesh(this->meshB))
		return false;

	// This is essential before we go into cutting one mesh against another, because the
	// algorthm that generates a mesh from a surface does not guarentee that all polygons
	// consist of coplanar vertices, and the algorithm that does the cutting assumes that
	// all polygons have coplanar vertices.
	this->meshA.TessellateFaces();
	this->meshB.TessellateFaces();

#else

	this->meshA.GeneratePolyhedron(PolygonMesh::Polyhedron::HEXADRON, 4.0);
	this->meshB.GeneratePolyhedron(PolygonMesh::Polyhedron::HEXADRON, 4.0);

	// STPTODO: Need to test all sorts of cases regarding how the boxes overlap.
	Transform transform;
	transform.translation.SetComponents(2.0, 2.0, 2.0);
	transform.matrix.SetFromAxisAngle(Vector3(1.0, 1.0, 1.0).Normalized(), M_PI / 3.0);
	transform.TransformMesh(this->meshB);
#endif
	
	if (!this->diffMesh.CalculateIntersection(this->meshA, this->meshB))
		return false;

	return true;
}

/*virtual*/ bool TestMeshSetOps::Render()
{
	if (!TestCase::Render())
		return false;

	if (this->renderMeshA)
		this->RenderMeshPolygons(this->meshA);

	if (this->renderMeshB)
		this->RenderMeshPolygons(this->meshB);

	if (this->renderDiffMesh)
		this->RenderMeshPolygons(this->diffMesh);

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

	if (controller->WasButtonPressed(XINPUT_GAMEPAD_B))
	{
		this->renderDiffMesh = !this->renderDiffMesh;
	}
}