#include "HappyMath/PolygonMesh.h"
#include "HappyMath/Polygon.h"
#include "HappyMath/Graph.h"
#include "HappyMath/ExpandingPolytopeAlgorithm.h"

using namespace HappyMath;

//--------------------------- PolygonMesh::Polygon ---------------------------

PolygonMesh::PolygonMesh()
{
}

PolygonMesh::PolygonMesh(const PolygonMesh& polygonMesh)
{
	*this = polygonMesh;
}

/*virtual*/ PolygonMesh::~PolygonMesh()
{
}

void PolygonMesh::operator=(const PolygonMesh& polygonMesh)
{
	this->Clear();

	for (const Polygon& polygon : polygonMesh.polygonArray)
		this->polygonArray.push_back(polygon);

	for (const Vector3& vertex : polygonMesh.vertexArray)
		this->vertexArray.push_back(vertex);
}

bool PolygonMesh::IsValid() const
{
	for (const Vector3& vertex : this->vertexArray)
		if (!vertex.IsValid())
			return false;

	for (const Polygon& polygon : this->polygonArray)
		for (int i : polygon.vertexArray)
			if (i < 0 || i >= (signed)this->vertexArray.size())
				return false;

	return true;
}

int PolygonMesh::FindVertex(const Vector3& vertex, double epsilon /*= 1e-6*/)
{
	// TODO: We might consider optimizing this with a spacial index.

	for (int i = 0; i < (signed)this->vertexArray.size(); i++)
		if (this->vertexArray[i].IsPoint(vertex, epsilon))
			return i;

	return -1;
}

int PolygonMesh::FindOrAddVertex(const Vector3& vertex, double epsilon /*= 1e-6*/)
{
	int i = this->FindVertex(vertex, epsilon);
	if (i >= 0)
		return i;

	return this->AddVertex(vertex);
}

int PolygonMesh::AddVertex(const Vector3& vertex)
{
	int i = (signed)this->vertexArray.size();
	this->vertexArray.push_back(vertex);
	return i;
}

void PolygonMesh::Clear()
{
	this->vertexArray.clear();
	this->polygonArray.clear();
}

bool PolygonMesh::GenerateConvexHull(const std::vector<Vector3>& pointArray)
{
	this->Clear();

	ExpandingPolytopeAlgorithm epa;
	ExpandingPolytopeAlgorithm::TypedTriangleFactory<ExpandingPolytopeAlgorithm::Triangle> triangleFactory(1024);

	auto findInitialTetrahedron = [&pointArray, &epa, &triangleFactory, this]() -> bool
	{
		// Is there a better approach to this problem?  This looks really
		// slow, but we only loop until we find a non-negative determinant.
		for (int i = 0; i < (signed)pointArray.size(); i++)
		{
			const Vector3& vertexA = pointArray[i];

			for (int j = 0; j < (signed)pointArray.size(); j++)
			{
				if (j == i)
					continue;

				const Vector3& vertexB = pointArray[j];

				for (int k = 0; k < (signed)pointArray.size(); k++)
				{
					if (k == i || k == j)
						continue;

					const Vector3& vertexC = pointArray[k];

					for (int l = 0; l < (signed)pointArray.size(); l++)
					{
						if (l == i || l == j || l == k)
							continue;

						const Vector3& vertexD = pointArray[l];

						Vector3 xAxis = vertexB - vertexA;
						Vector3 yAxis = vertexC - vertexA;
						Vector3 zAxis = vertexD - vertexA;

						double det = xAxis.Cross(yAxis).Dot(zAxis);

						if (det != 0.0)
						{
							epa.vertexArray.push_back(vertexA);
							epa.vertexArray.push_back(vertexB);
							epa.vertexArray.push_back(vertexC);
							epa.vertexArray.push_back(vertexD);
						}

						if (det < 0.0)
						{
							epa.triangleList.push_back(triangleFactory.AllocTriangle(0, 1, 2));
							epa.triangleList.push_back(triangleFactory.AllocTriangle(0, 2, 3));
							epa.triangleList.push_back(triangleFactory.AllocTriangle(0, 3, 1));
							epa.triangleList.push_back(triangleFactory.AllocTriangle(1, 3, 2));

							return true;
						}
						else if (det > 0.0)
						{
							epa.triangleList.push_back(triangleFactory.AllocTriangle(0, 2, 1));
							epa.triangleList.push_back(triangleFactory.AllocTriangle(0, 3, 2));
							epa.triangleList.push_back(triangleFactory.AllocTriangle(0, 1, 3));
							epa.triangleList.push_back(triangleFactory.AllocTriangle(1, 2, 3));

							return true;
						}
					}
				}
			}
		}

		return false;
	};

	if (!findInitialTetrahedron())
		return false;

	ExpandingPolytopeAlgorithm::PointListSupplier pointSupplier;
	for (const Vector3& point : pointArray)
		pointSupplier.pointList.push_back(point);

	epa.Expand(&pointSupplier, &triangleFactory);

	this->vertexArray = epa.vertexArray;

	for (const ExpandingPolytopeAlgorithm::Triangle* triangle : epa.triangleList)
	{
		Polygon polygon;
		for (int i = 0; i < 3; i++)
			polygon.vertexArray.push_back(triangle->vertex[i]);

		this->polygonArray.push_back(polygon);
	}

	return true;
}

bool PolygonMesh::ReduceEdgeCount(int numEdgesToRemove)
{
	if (numEdgesToRemove <= 0)
		return true;

	Graph graph;
	if (!graph.FromPolygohMesh(*this))
		return false;

	graph.ReduceEdgeCount(numEdgesToRemove);

	if (!graph.ToPolygonMesh(*this))
		return false;

	return true;
}

void PolygonMesh::CalculateUnion(const PolygonMesh& polygonMeshA, const PolygonMesh& polygonMeshB)
{
	// TODO: Write this.
}

void PolygonMesh::CalculateIntersection(const PolygonMesh& polygonMeshA, const PolygonMesh& polygonMeshB)
{
	// TODO: Write this.
}

void PolygonMesh::CalculateDifference(const PolygonMesh& polygonMeshA, const PolygonMesh& polygonMeshB)
{
	// TODO: Write this.
}

void PolygonMesh::SimplifyFaces(bool mustBeConvex, double epsilon /*= 1e-6*/)
{
	std::vector<HappyMath::Polygon> standalonePolygonArray;
	this->ToStandalonePolygonArray(standalonePolygonArray);

	HappyMath::Polygon::Compress(standalonePolygonArray, mustBeConvex);

	this->FromStandalonePolygonArray(standalonePolygonArray, epsilon);
}

void PolygonMesh::Reduce()
{
	int i = 0;
	while (i < (signed)this->polygonArray.size())
	{
		Polygon& polygon = this->polygonArray[i];
		if (polygon.vertexArray.size() >= 3)
			i++;
		else
		{
			if (i != this->polygonArray.size() - 1)
				this->polygonArray[i] = this->polygonArray[this->polygonArray.size() - 1];
			this->polygonArray.pop_back();
		}
	}
}

void PolygonMesh::TessellateFaces(double epsilon /*= 1e-6*/)
{
	std::vector<HappyMath::Polygon> standalonePolygonArrayA;
	this->ToStandalonePolygonArray(standalonePolygonArrayA);

	std::vector<HappyMath::Polygon> standalonePolygonArrayB;
	for(HappyMath::Polygon& polygon : standalonePolygonArrayA)
		polygon.TessellateUntilTriangular(standalonePolygonArrayB);

	this->FromStandalonePolygonArray(standalonePolygonArrayB, epsilon);
}

void PolygonMesh::ToStandalonePolygonArray(std::vector<HappyMath::Polygon>& standalonePolygonArray) const
{
	for (const Polygon& polygon : this->polygonArray)
	{
		HappyMath::Polygon standalonePolygon;
		polygon.ToStandalonePolygon(standalonePolygon, this);
		standalonePolygonArray.push_back(standalonePolygon);
	}
}

void PolygonMesh::FromStandalonePolygonArray(const std::vector<HappyMath::Polygon>& standalonePolygonArray, double epsilon /*= 1e-6*/)
{
	this->Clear();

	for (const HappyMath::Polygon& standalonePolygon : standalonePolygonArray)
	{
		Polygon polygon;
		polygon.FromStandalonePolygon(standalonePolygon, this, epsilon);
		this->polygonArray.push_back(polygon);
	}
}

bool PolygonMesh::RayCast(const Ray& ray, double& alpha, Vector3& unitSurfaceNormal) const
{
	alpha = std::numeric_limits<double>::max();

	for (const Polygon& polygon : this->polygonArray)
	{
		HappyMath::Polygon standalonePolygon;
		polygon.ToStandalonePolygon(standalonePolygon, this);
		double polygonAlpha = 0.0;
		Vector3 polygonNormal;
		if (standalonePolygon.RayCast(ray, polygonAlpha, polygonNormal))
		{
			if (polygonAlpha < alpha)
			{
				alpha = polygonAlpha;
				unitSurfaceNormal = polygonNormal;
			}
		}
	}

	return alpha != std::numeric_limits<double>::max();
}

void PolygonMesh::Dump(std::ostream& stream) const
{
	uint32_t numVertices = (uint32_t)this->vertexArray.size();
	stream.write((char*)&numVertices, sizeof(numVertices));

	uint32_t numPolygons = (uint32_t)this->polygonArray.size();
	stream.write((char*)&numPolygons, sizeof(numPolygons));

	for (const Vector3& vertex : this->vertexArray)
		vertex.Dump(stream);

	for (const Polygon& polygon : this->polygonArray)
		polygon.Dump(stream);
}

void PolygonMesh::Restore(std::istream& stream)
{
	uint32_t numVertices = 0;
	stream.read((char*)&numVertices, sizeof(numVertices));

	uint32_t numPolygons = 0;
	stream.read((char*)&numPolygons, sizeof(numPolygons));

	this->Clear();

	for (int i = 0; i < (signed)numVertices; i++)
	{
		Vector3 vertex;
		vertex.Restore(stream);
		this->vertexArray.push_back(vertex);
	}

	for (int i = 0; i < (signed)numPolygons; i++)
	{
		Polygon polygon;
		polygon.Restore(stream);
		this->polygonArray.push_back(polygon);
	}
}

Vector3 PolygonMesh::CalcVertexAverage() const
{
	Vector3 average(0.0, 0.0, 0.0);

	if (this->vertexArray.size() > 0)
	{
		for (const Vector3& vertex : this->vertexArray)
			average += vertex;

		average /= double(this->vertexArray.size());
	}

	return average;
}

//--------------------------- PolygonMesh::Polygon ---------------------------

PolygonMesh::Polygon::Polygon()
{
}

PolygonMesh::Polygon::Polygon(const Polygon& polygon)
{
	*this = polygon;
}

/*virtual*/ PolygonMesh::Polygon::~Polygon()
{
}

void PolygonMesh::Polygon::operator=(const Polygon& polygon)
{
	this->Clear();

	for (int i : polygon.vertexArray)
		this->vertexArray.push_back(i);
}

int PolygonMesh::Polygon::operator()(int i) const
{
	i %= (signed)this->vertexArray.size();
	if (i < 0)
		i += int(this->vertexArray.size());
	return this->vertexArray[i];
}

void PolygonMesh::Polygon::Clear()
{
	this->vertexArray.clear();
}

void PolygonMesh::Polygon::ToStandalonePolygon(HappyMath::Polygon& polygon, const PolygonMesh* mesh) const
{
	polygon.Clear();

	for (int i : this->vertexArray)
		polygon.vertexArray.push_back(mesh->vertexArray[i]);
}

void PolygonMesh::Polygon::FromStandalonePolygon(const HappyMath::Polygon& polygon, PolygonMesh* mesh, double epsilon /*= 1e-6*/)
{
	this->Clear();

	for (const Vector3& vertex : polygon.vertexArray)
		this->vertexArray.push_back(mesh->FindOrAddVertex(vertex, epsilon));
}

int PolygonMesh::Polygon::Mod(int i) const
{
	i %= (int)this->vertexArray.size();
	if (i < 0)
		i += (int)this->vertexArray.size();
	return i;
}

void PolygonMesh::Polygon::Reverse()
{
	std::vector<int> vertexStack;
	for (int i : this->vertexArray)
		vertexStack.push_back(i);

	this->vertexArray.clear();
	while (vertexStack.size() > 0)
	{
		int i = vertexStack[vertexStack.size() - 1];
		vertexStack.pop_back();
		this->vertexArray.push_back(i);
	}
}

bool PolygonMesh::Polygon::HasVertex(int vertex) const
{
	for (int i = 0; i < (int)this->vertexArray.size(); i++)
		if (this->vertexArray[i] == vertex)
			return true;

	return false;
}

void PolygonMesh::Polygon::Dump(std::ostream& stream) const
{
	uint32_t numVertices = (uint32_t)this->vertexArray.size();
	stream.write((char*)&numVertices, sizeof(numVertices));

	for (int i : this->vertexArray)
		stream.write((char*)&i, sizeof(i));
}

void PolygonMesh::Polygon::Restore(std::istream& stream)
{
	uint32_t numVertices = 0;
	stream.read((char*)&numVertices, sizeof(numVertices));

	this->vertexArray.clear();
	for (int i = 0; i < (signed)numVertices; i++)
	{
		int j = 0;
		stream.read((char*)&j, sizeof(j));
		this->vertexArray.push_back(j);
	}
}