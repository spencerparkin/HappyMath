#include "App.h"
#include "HappyMath/Surface.h"
#include "HappyMath/Frustum.h"

using namespace HappyMath;

App::App()
{
	this->window = nullptr;
	this->context = nullptr;
	this->appSetup = false;
	this->cameraEye.SetComponents(0.0, 0.0, 40.0);
	this->lastTickTime = 0;
	this->draggingMouse = false;
}

/*virtual*/ App::~App()
{
}

bool App::Setup()
{
	if (this->appSetup)
		return false;

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("Failed to init video: %s", SDL_GetError());
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	this->window = SDL_CreateWindow("Happy Math Test App", 2000, 1500, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!this->window)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}

	this->context = SDL_GL_CreateContext(this->window);
	if (!this->context)
	{
		SDL_Log("Failed to create OpenGL context: %s", SDL_GetError());
		return false;
	}

	SDL_GL_SetSwapInterval(1);	// Enable V-sync.

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

	this->lastTickTime = SDL_GetTicksNS();

	this->appSetup = true;
	return true;
}

void App::Shutdown()
{
	if (this->context)
	{
		SDL_GL_DestroyContext(this->context);
		this->context = nullptr;
	}

	if (this->window)
	{
		SDL_DestroyWindow(this->window);
		this->window = nullptr;
	}

	SDL_Quit();
	this->appSetup = false;
}

bool App::Run()
{
	Uint64 currentTickTime = SDL_GetTicksNS();
	Uint64 deltaTicks = currentTickTime - this->lastTickTime;
	this->lastTickTime = currentTickTime;
	double deltaTimeSeconds = double(deltaTicks) / 1'000'000'000.0;

	// Prevent debugger pauses from being an issue.
	if (deltaTimeSeconds > 0.5)
		deltaTimeSeconds = 0.0;

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT)
			return false;

		this->HandleEvent(event, deltaTimeSeconds);
	}

	this->Render(deltaTimeSeconds);

	return true;
}

void App::Render(double deltaTimeSeconds)
{
	int windowWidth = 0, windowHeight = 0;
	SDL_GetWindowSizeInPixels(window, &windowWidth, &windowHeight);

	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	double aspectRatio = double(windowWidth) / double(windowHeight);

	HappyMath::Frustum frustum;
	frustum.SetFromAspectRatio(aspectRatio, M_PI / 3.0, 0.1, 1000.0);

	HappyMath::Matrix4x4 projMatrix;
	frustum.GetToProjectionMatrix(projMatrix);

	HappyMath::Matrix4x4 projMatrixT;
	projMatrixT.Transpose(projMatrix);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixd(&projMatrixT.ele[0][0]);

	this->viewToWorld.SetAsViewToWorldTransform(this->cameraEye, Vector3(0.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0));

	HappyMath::Matrix4x4 worldToView;
	worldToView.Invert(viewToWorld);

	HappyMath::Matrix4x4 worldToViewT;
	worldToViewT.Transpose(worldToView);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixd(&worldToViewT.ele[0][0]);

	glBegin(GL_LINES);

	// X-axis.
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	// Y-axis.
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	// Z-axis.
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);

	glEnd();

	glBegin(GL_POINTS);
	glPointSize(2.0f);
	glColor3f(1.0f, 0.0f, 0.0f);

	for (int i = 0; i < this->graph.GetNumNodes(); i++)
	{
		const Graph::Node* node = this->graph.GetNode(i);
		const Vector3& vertex = node->GetVertex();
		glVertex3d(vertex.x, vertex.y, vertex.z);
	}

	glEnd();

	glBegin(GL_LINES);
	glColor3f(1.0f, 1.0f, 1.0f);

	for (const Graph::UnorderedEdge& edge : this->edgeSet)
	{
		const Graph::Node* nodeA = this->graph.GetNode(edge.i);
		const Graph::Node* nodeB = this->graph.GetNode(edge.j);

		const Vector3& vertexA = nodeA->GetVertex();
		const Vector3& vertexB = nodeB->GetVertex();

		glVertex3d(vertexA.x, vertexA.y, vertexA.z);
		glVertex3d(vertexB.x, vertexB.y, vertexB.z);
	}

	glEnd();

	glBegin(GL_TRIANGLES);

	double r = 0.1;
	double g = 0.2;
	double b = 0.3;

	for (int i = 0; i < this->mesh.GetNumPolygons(); i++)
	{
		const PolygonMesh::Polygon& polygon = this->mesh.GetPolygon(i);
		if (polygon.vertexArray.size() != 3)
			continue;

		glColor3d(r, g, b);

		r = ::fmod(r + 0.59, 1.0);
		g = ::fmod(r + 0.72, 1.0);
		b = ::fmod(r + 0.27, 1.0);

		for (int j = 0; j < (int)polygon.vertexArray.size(); j++)
		{
			const Vector3& vertex = this->mesh.GetVertex(polygon.vertexArray[j]);
			glVertex3d(vertex.x, vertex.y, vertex.z);
		}
	}

	glEnd();

	SDL_GL_SwapWindow(this->window);
}

void App::HandleEvent(SDL_Event& event, double deltaTimeSeconds)
{
	switch (event.type)
	{
		case SDL_EVENT_MOUSE_MOTION:
		{
			if (this->draggingMouse)
			{
				Vector2 currentMousePos(event.motion.x, event.motion.y);
				Vector2 mouseDelta = currentMousePos - this->lastMousePos;
				this->lastMousePos = currentMousePos;

				double distanceToOrigin = this->cameraEye.Length();

				Vector3 xAxis, yAxis, zAxes;
				this->viewToWorld.GetAxes(xAxis, yAxis, zAxes);

				double mouseSensativity = 0.03;

				this->cameraEye += mouseSensativity * (-mouseDelta.x * xAxis + mouseDelta.y * yAxis);
				this->cameraEye *= distanceToOrigin / this->cameraEye.Length();
			}

			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				this->draggingMouse = true;
				this->lastMousePos.x = event.motion.x;
				this->lastMousePos.y = event.motion.y;
			}
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			if (event.button.button == SDL_BUTTON_LEFT)
				this->draggingMouse = false;
			break;
		}
		case SDL_EVENT_MOUSE_WHEEL:
		{
			double distanceToOrigin = this->cameraEye.Length();

			double zoomFactor = (event.wheel.y > 0.0f) ? 0.9 : 1.1;

			for (int i = 0; i < (int)::fabs(event.wheel.y); i++)
				distanceToOrigin *= zoomFactor;

			this->cameraEye *= distanceToOrigin / this->cameraEye.Length();

			break;
		}
	}
}