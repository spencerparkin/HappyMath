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
	this->meshA.GeneratePolyhedron(PolygonMesh::Polyhedron::HEXADRON, 4.0);
	this->meshB.GeneratePolyhedron(PolygonMesh::Polyhedron::HEXADRON, 4.0);

	// STPTODO: Need to test all sorts of cases regarding how the boxes overlap.
	Transform transform;
	transform.translation.SetComponents(2.0, 2.0, 2.0);
	transform.TransformMesh(this->meshB);
	
	if (!this->diffMesh.CalculateUnion(this->meshA, this->meshB))
		return false;

	return true;
}

/*virtual*/ bool TestMeshSetOps::Render()
{
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