#include "Test.h"
#include "HappyMath/Graph.h"
#include "HappyMath/Surface.h"
#include "HappyMath/PolygonMesh.h"
#include <fstream>

using namespace HappyMath;

int main(int argc, char** argv)
{
	SphereSurface surface(Vector3(0.0, 0.0, 0.0), 10.0);

	Graph graph;
	if (!graph.FromSurface(&surface, 3, 1.0, Vector3(1.0, 0.0, 0.0)))
		return -1;

	std::ofstream fileStream;
	fileStream.open("graph.bin", std::ios::out | std::ios::binary);
	if (!fileStream.is_open())
		return false;

	graph.Dump(fileStream);
	fileStream.close();

#if 0
	PolygonMesh mesh;
	if (!graph.ToPolygonMesh(mesh))
		return -1;

	// STPTODO: Tessellate polygons into triangles here?
	
	fileStream.open("sphere_mesh.bin", std::ios::out | std::ios::binary);
	if (!fileStream.is_open())
		return false;

	mesh.Dump(fileStream);
	fileStream.close();
#endif

	return 0;
}