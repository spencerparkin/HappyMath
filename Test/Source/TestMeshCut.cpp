#include "TestMeshCut.h"
#include "HappyMath/Surface.h"
#include "HappyMath/LineSegment.h"
#include "HappyMath/Polygon.h"
#include "HappyMath/Transform.h"
#include "XBoxController.h"
#include <SDL3/SDL_opengl.h>

using namespace HappyMath;

TestMeshCut::TestMeshCut()
{
	this->renderMeshA = true;
	this->renderMeshB = true;

	this->hackOffset = 0;
}

/*virtual*/ TestMeshCut::~TestMeshCut()
{
}

/*virtual*/ bool TestMeshCut::Setup()
{
#if 1
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
#else

	PolygonMesh meshA, meshB;

	meshA.GeneratePolyhedron(PolygonMesh::Polyhedron::HEXADRON, 4.0);
	meshB.GeneratePolyhedron(PolygonMesh::Polyhedron::HEXADRON, 4.0);

	Transform transform;
	transform.translation.SetComponents(2.0, 2.0, 2.0);
	transform.matrix.SetFromAxisAngle(Vector3(1.0, 1.0, 1.0).Normalized(), M_PI / 3.0);
	transform.TransformMesh(meshB);

	/*
	HappyMath::Polygon polygon;
	meshA.GetPolygon(10).ToStandalonePolygon(polygon, &meshA);
	meshA.Clear();
	std::vector<HappyMath::Polygon> polygonArray = { polygon };
	meshA.FromStandalonePolygonArray(polygonArray);

	meshB.GetPolygon(0).ToStandalonePolygon(polygon, &meshB);
	meshB.Clear();
	polygonArray = { polygon };
	meshB.FromStandalonePolygonArray(polygonArray);
	*/
#endif

	std::vector<HappyMath::Polygon> polygonArrayA, polygonArrayB;

	if (!PolygonMesh::CalculateCutPolygons(meshA, meshB, polygonArrayA, polygonArrayB, this->cutSegmentArray))
		return false;
	
	this->cutMeshA.FromStandalonePolygonArray(polygonArrayA);
	this->cutMeshB.FromStandalonePolygonArray(polygonArrayB);

	return true;
}

/*virtual*/ bool TestMeshCut::Render()
{
	if (!TestCase::Render())
		return false;

	if (this->renderMeshA)
		this->RenderMeshPolygons(this->cutMeshA, -1 /*this->hackOffset*/);

	if (this->renderMeshB)
		this->RenderMeshPolygons(this->cutMeshB);

	this->RenderLineSegments(this->cutSegmentArray);

	return true;
}

/*virtual*/ void TestMeshCut::HandleController(XBoxController* controller)
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
		this->hackOffset++;
	}
}