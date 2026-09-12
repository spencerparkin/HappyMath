#include "TestSurfaceUnion.h"
#include "HappyMath/Surface.h"
#include <SDL3/SDL_opengl.h>

using namespace HappyMath;

TestSurfaceUnion::TestSurfaceUnion()
{
}

/*virtual*/ TestSurfaceUnion::~TestSurfaceUnion()
{
}

/*virtual*/ bool TestSurfaceUnion::Setup()
{
	auto* ellipticalSurface = new EllipticalDonutSurface();
	ellipticalSurface->A = 10.0;
	ellipticalSurface->B = 14.0;
	ellipticalSurface->girthRadius = 3.0;

	auto* sphericalSurface = new SphereSurface(Vector3(8.0, 0.0, 0.0), 10.0);

	// This was an interesting experiment.  It seems to have worked for the most part,
	// but as expected, the graph algorithm suffers where the surface is not well-behaved.
	// (i.e., where the surface has discontinuities in the gradient.)  Also, I can't be
	// sure that the FindNearestPoint function is actually correct in all cases.
	UnionSurface surface(ellipticalSurface, sphericalSurface);

	if (!this->graph.FromSurface(&surface, 5, 1.0, Vector3(1.0, 0.0, 0.0)))
		return false;

	this->graph.GenerateEdgeSet(this->edgeSet);

	if (!this->graph.ToPolygonMesh(this->mesh))
		return false;

	if (!this->mesh.TessellateFaces())
		return false;

	return true;
}

/*virtual*/ bool TestSurfaceUnion::Render()
{
	this->RenderGraphNodes(&this->graph, Vector3(0.0, 1.0, 0.0));

	this->RenderEdgeSet(this->edgeSet, &this->graph, Vector3(1.0, 1.0, 1.0));

	this->RenderMeshTriangles(this->mesh);

	return true;
}