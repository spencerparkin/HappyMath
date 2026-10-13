#include "TestCase.h"
#include "HappyMath/Polygon.h"
#include "HappyMath/LineSegment.h"
#include <SDL3/SDL_opengl.h>

using namespace HappyMath;

TestCase::TestCase()
{
	this->colorOffset = 0;

	this->colorArray.push_back(Vector3(1.0, 0.0, 0.0));
	this->colorArray.push_back(Vector3(0.0, 1.0, 0.0));
	this->colorArray.push_back(Vector3(0.0, 0.0, 1.0));
	this->colorArray.push_back(Vector3(1.0, 1.0, 0.0));
	this->colorArray.push_back(Vector3(0.0, 1.0, 1.0));
	this->colorArray.push_back(Vector3(1.0, 0.0, 1.0));
	this->colorArray.push_back(Vector3(1.0, 0.5, 0.0));
	this->colorArray.push_back(Vector3(0.0, 1.0, 0.5));
	this->colorArray.push_back(Vector3(0.5, 0.0, 1.0));
	this->colorArray.push_back(Vector3(1.0, 1.0, 0.5));
	this->colorArray.push_back(Vector3(0.5, 1.0, 1.0));
	this->colorArray.push_back(Vector3(1.0, 0.5, 1.0));
}

/*virtual*/ TestCase::~TestCase()
{
}

/*virtual*/ bool TestCase::Setup()
{
	return true;
}

/*virtual*/ bool TestCase::Shutdown()
{
	return true;
}

/*virtual*/ bool TestCase::Render()
{
	this->colorOffset = 0;

	return true;
}

void TestCase::RenderGraphNodes(const HappyMath::Graph* graph, const HappyMath::Vector3& color)
{
	glBegin(GL_POINTS);
	glPointSize(2.0f);
	glColor3f(1.0f, 0.0f, 0.0f);

	for (int i = 0; i < graph->GetNumNodes(); i++)
	{
		const Graph::Node* node = graph->GetNode(i);
		const Vector3& vertex = node->GetVertex();
		glVertex3d(vertex.x, vertex.y, vertex.z);
	}

	glEnd();
}

void TestCase::RenderEdgeSet(const std::set<HappyMath::Graph::UnorderedEdge, HappyMath::Graph::UnorderedEdge>& edgeSet, const HappyMath::Graph* graph, const HappyMath::Vector3& color)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);

	for (const Graph::UnorderedEdge& edge : edgeSet)
	{
		const Graph::Node* nodeA = graph->GetNode(edge.i);
		const Graph::Node* nodeB = graph->GetNode(edge.j);

		const Vector3& vertexA = nodeA->GetVertex();
		const Vector3& vertexB = nodeB->GetVertex();

		glVertex3d(vertexA.x, vertexA.y, vertexA.z);
		glVertex3d(vertexB.x, vertexB.y, vertexB.z);
	}

	glEnd();
}

const HappyMath::Vector3& TestCase::GetNextColor()
{
	this->colorOffset = (this->colorOffset + 1) % this->colorArray.size();

	return this->colorArray[this->colorOffset];
}

void TestCase::RenderMeshTriangles(const HappyMath::PolygonMesh& polygonMesh)
{
	glBegin(GL_TRIANGLES);

	for (int i = 0; i < polygonMesh.GetNumPolygons(); i++)
	{
		const PolygonMesh::Polygon& polygon = polygonMesh.GetPolygon(i);
		if (polygon.vertexArray.size() != 3)
			continue;

		const Vector3& color = this->GetNextColor();
		glColor3dv(&color.x);

		for (int j = 0; j < (int)polygon.vertexArray.size(); j++)
		{
			const Vector3& vertex = polygonMesh.GetVertex(polygon.vertexArray[j]);
			glVertex3d(vertex.x, vertex.y, vertex.z);
		}
	}

	glEnd();
}

void TestCase::RenderMeshPolygons(const HappyMath::PolygonMesh& polygonMesh, int isolatedPolygon /*= -1*/)
{
	for (int i = 0; i < polygonMesh.GetNumPolygons(); i++)
	{
		const Vector3& color = this->GetNextColor();

		if (isolatedPolygon != -1 && i != isolatedPolygon)
			continue;

		const PolygonMesh::Polygon& polygon = polygonMesh.GetPolygon(i);

		glBegin(GL_POLYGON);
		glColor3dv(&color.x);

		for (int j = 0; j < (int)polygon.vertexArray.size(); j++)
		{
			const Vector3& vertex = polygonMesh.GetVertex(polygon.vertexArray[j]);
			glVertex3d(vertex.x, vertex.y, vertex.z);
		}

		glEnd();
	}
}

void TestCase::RenderLineSegments(const std::vector<HappyMath::LineSegment>& lineSegmentArray)
{
	glLineWidth(2.0f);
	glBegin(GL_LINES);

	for (int i = 0; i < (int)lineSegmentArray.size(); i++)
	{
		const HappyMath::LineSegment& lineSegment = lineSegmentArray[i];

		const Vector3& color = this->GetNextColor();
		glColor3dv(&color.x);

		glVertex3dv(&lineSegment.point[0].x);
		glVertex3dv(&lineSegment.point[1].x);
	}

	glEnd();
}

void TestCase::RenderPolygonEdges(const HappyMath::Polygon& polygon, const HappyMath::Vector3& color)
{
	glBegin(GL_LINE_LOOP);

	glColor3d(color.x, color.y, color.z);

	for (int i = 0; i < (int)polygon.vertexArray.size(); i++)
	{
		const Vector3& vertex = polygon.vertexArray[i];

		glVertex3d(vertex.x, vertex.y, vertex.z);
	}

	glEnd();
}

void TestCase::RenderPolygonVertices(const HappyMath::Polygon& polygon, const HappyMath::Vector3& color)
{
	glPointSize(4.0f);
	glBegin(GL_POINTS);

	for (int i = 0; i < (int)polygon.vertexArray.size(); i++)
	{
		const Vector3& vertex = polygon.vertexArray[i];

		double mask = 0.1 + 0.9 * (double(i) / double(polygon.vertexArray.size() - 1));

		Vector3 actualColor = mask * color;

		glColor3d(actualColor.x, actualColor.y, actualColor.z);
		glVertex3d(vertex.x, vertex.y, vertex.z);
	}

	glEnd();
}

/*virtual*/ void TestCase::HandleController(XBoxController* controller)
{
}